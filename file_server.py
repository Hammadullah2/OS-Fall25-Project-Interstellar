import socket
import struct
import hashlib
import sys
import os

# Configuration
SERVER_PORT = 9999
SERVER_HOST = '0.0.0.0'  # Listen on all interfaces
CHUNK_SIZE = 1024  # Data payload per packet (1KB)

# File identifiers
FILE_MODEL = 0
FILE_TOKENIZER = 1

# Protocol message types
MSG_METADATA_REQUEST = 1
MSG_METADATA_RESPONSE = 2
MSG_DATA_REQUEST = 3
MSG_DATA_RESPONSE = 4
MSG_ERROR = 5

class FileServer:
    def __init__(self, model_path, tokenizer_path):
        """Initialize the file server with file paths"""
        self.files = {}
        self.sock = None
        
        # Load files
        print(f"Loading model from: {model_path}")
        self.load_file(FILE_MODEL, model_path)
        
        print(f"Loading tokenizer from: {tokenizer_path}")
        self.load_file(FILE_TOKENIZER, tokenizer_path)
        
        print(f"\nServer initialized successfully")
        print(f"Model size: {len(self.files[FILE_MODEL]['data'])} bytes")
        print(f"Model SHA-256: {self.files[FILE_MODEL]['checksum'].hex()}")
        print(f"Tokenizer size: {len(self.files[FILE_TOKENIZER]['data'])} bytes")
        print(f"Tokenizer SHA-256: {self.files[FILE_TOKENIZER]['checksum'].hex()}")
        
    def load_file(self, file_id, filepath):
        """Load a file and compute its SHA-256 checksum"""
        if not os.path.exists(filepath):
            print(f"ERROR: File not found: {filepath}")
            sys.exit(1)
            
        with open(filepath, 'rb') as f:
            data = f.read()
        
        # Compute SHA-256 checksum
        checksum = hashlib.sha256(data).digest()
        
        # Calculate number of chunks needed
        num_chunks = (len(data) + CHUNK_SIZE - 1) // CHUNK_SIZE
        
        self.files[file_id] = {
            'data': data,
            'checksum': checksum,
            'size': len(data),
            'num_chunks': num_chunks,
            'name': os.path.basename(filepath)
        }
        
    def handle_metadata_request(self, file_id):
        """Handle metadata request - return file size, chunk count, and SHA-256"""
        if file_id not in self.files:
            return self.create_error_response(f"Invalid file ID: {file_id}")
        
        file_info = self.files[file_id]
        
        # Response format:
        # 1 byte: message type (MSG_METADATA_RESPONSE)
        # 1 byte: file_id
        # 4 bytes: file size
        # 4 bytes: number of chunks
        # 32 bytes: SHA-256 checksum
        response = struct.pack('!BB', MSG_METADATA_RESPONSE, file_id)
        response += struct.pack('!II', file_info['size'], file_info['num_chunks'])
        response += file_info['checksum']
        
        print(f"Metadata request for file {file_id} ({file_info['name']})")
        print(f"  Size: {file_info['size']} bytes, Chunks: {file_info['num_chunks']}")
        
        return response
    
    def handle_data_request(self, file_id, chunk_index):
        """Handle data request - return specific chunk of file"""
        if file_id not in self.files:
            return self.create_error_response(f"Invalid file ID: {file_id}")
        
        file_info = self.files[file_id]
        
        if chunk_index >= file_info['num_chunks']:
            return self.create_error_response(f"Invalid chunk index: {chunk_index}")
        
        # Calculate chunk boundaries
        start_offset = chunk_index * CHUNK_SIZE
        end_offset = min(start_offset + CHUNK_SIZE, file_info['size'])
        chunk_data = file_info['data'][start_offset:end_offset]
        chunk_size = len(chunk_data)
        
        # Response format:
        # 1 byte: message type (MSG_DATA_RESPONSE)
        # 1 byte: file_id
        # 4 bytes: chunk_index
        # 2 bytes: chunk_size
        # N bytes: chunk data
        response = struct.pack('!BB', MSG_DATA_RESPONSE, file_id)
        response += struct.pack('!IH', chunk_index, chunk_size)
        response += chunk_data
        
        # Print progress every 100 chunks
        if chunk_index % 100 == 0 or chunk_index == file_info['num_chunks'] - 1:
            print(f"Data chunk {chunk_index}/{file_info['num_chunks']-1} for file {file_id}")
        
        return response
    
    def create_error_response(self, error_msg):
        """Create an error response"""
        print(f"ERROR: {error_msg}")
        # Simple error response: MSG_ERROR + error string (truncated to 256 bytes)
        error_bytes = error_msg.encode('utf-8')[:256]
        response = struct.pack('!B', MSG_ERROR)
        response += error_bytes
        return response
    
    def handle_request(self, data, addr):
        """Parse and handle incoming request"""
        if len(data) < 2:
            return self.create_error_response("Request too short")
        
        msg_type = data[0]
        
        try:
            if msg_type == MSG_METADATA_REQUEST:
                # Format: 1 byte msg_type, 1 byte file_id
                file_id = data[1]
                return self.handle_metadata_request(file_id)
                
            elif msg_type == MSG_DATA_REQUEST:
                # Format: 1 byte msg_type, 1 byte file_id, 4 bytes chunk_index
                if len(data) < 6:
                    return self.create_error_response("Data request too short")
                file_id = data[1]
                chunk_index = struct.unpack('!I', data[2:6])[0]
                return self.handle_data_request(file_id, chunk_index)
                
            else:
                return self.create_error_response(f"Unknown message type: {msg_type}")
                
        except Exception as e:
            return self.create_error_response(f"Error processing request: {str(e)}")
    
    def run(self):
        """Main server loop"""
        # Create UDP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        # Bind to port
        self.sock.bind((SERVER_HOST, SERVER_PORT))
        print(f"\nServer listening on {SERVER_HOST}:{SERVER_PORT}")
        print("Waiting for requests from xv6...\n")
        
        try:
            while True:
                # Receive request
                data, addr = self.sock.recvfrom(4096)
                
                # Handle request and send response
                response = self.handle_request(data, addr)
                self.sock.sendto(response, addr)
                
        except KeyboardInterrupt:
            print("\n\nServer shutting down...")
        finally:
            if self.sock:
                self.sock.close()

def main():
    """Main entry point"""
    # Parse command line arguments
    if len(sys.argv) == 3:
        model_path = sys.argv[1]
        tokenizer_path = sys.argv[2]
    else:
        # Default paths - adjust these to match your file locations
        model_path = "stories15M.bin"
        tokenizer_path = "tokenizer.bin"
        
        print(f"Usage: {sys.argv[0]} <model_path> <tokenizer_path>")
        print(f"Using default paths:")
        print(f"  Model: {model_path}")
        print(f"  Tokenizer: {tokenizer_path}")
        print()
    
    # Create and run server
    server = FileServer(model_path, tokenizer_path)
    server.run()

if __name__ == '__main__':
    main()
