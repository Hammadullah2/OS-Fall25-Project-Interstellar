
import socket
import struct
import hashlib

# Configuration
SERVER_HOST = '127.0.0.1'
SERVER_PORT = 9999

# Protocol constants
MSG_METADATA_REQUEST = 1
MSG_METADATA_RESPONSE = 2
MSG_DATA_REQUEST = 3
MSG_DATA_RESPONSE = 4
MSG_ERROR = 5

FILE_MODEL = 0
FILE_TOKENIZER = 1

def request_metadata(sock, file_id):
    """Request file metadata"""
    request = struct.pack('!BB', MSG_METADATA_REQUEST, file_id)
    sock.sendto(request, (SERVER_HOST, SERVER_PORT))
    
    response, _ = sock.recvfrom(4096)
    
    if response[0] == MSG_ERROR:
        print(f"Error: {response[1:].decode('utf-8', errors='ignore')}")
        return None
    
    if response[0] != MSG_METADATA_RESPONSE:
        print(f"Unexpected response type: {response[0]}")
        return None
    
    # Parse response
    msg_type, file_id = struct.unpack('!BB', response[0:2])
    file_size, num_chunks = struct.unpack('!II', response[2:10])
    checksum = response[10:42]
    
    return {
        'file_id': file_id,
        'size': file_size,
        'num_chunks': num_chunks,
        'checksum': checksum
    }

def request_chunk(sock, file_id, chunk_index):
    """Request a specific data chunk"""
    request = struct.pack('!BBI', MSG_DATA_REQUEST, file_id, chunk_index)
    sock.sendto(request, (SERVER_HOST, SERVER_PORT))
    
    response, _ = sock.recvfrom(4096)
    
    if response[0] == MSG_ERROR:
        print(f"Error: {response[1:].decode('utf-8', errors='ignore')}")
        return None
    
    if response[0] != MSG_DATA_RESPONSE:
        print(f"Unexpected response type: {response[0]}")
        return None
    
    # Parse response
    msg_type, file_id = struct.unpack('!BB', response[0:2])
    chunk_idx, chunk_size = struct.unpack('!IH', response[2:8])
    chunk_data = response[8:8+chunk_size]
    
    return chunk_data

def download_file(sock, file_id, file_name):
    """Download complete file and verify checksum"""
    print(f"\n=== Downloading {file_name} (file_id={file_id}) ===")
    
    # Get metadata
    print("Requesting metadata...")
    metadata = request_metadata(sock, file_id)
    if not metadata:
        print("Failed to get metadata")
        return False
    
    print(f"File size: {metadata['size']} bytes")
    print(f"Chunks: {metadata['num_chunks']}")
    print(f"Expected SHA-256: {metadata['checksum'].hex()}")
    
    # Download all chunks
    file_data = bytearray()
    print("Downloading chunks...")
    
    for chunk_idx in range(metadata['num_chunks']):
        if chunk_idx % 100 == 0:
            print(f"  Progress: {chunk_idx}/{metadata['num_chunks']}")
        
        chunk = request_chunk(sock, file_id, chunk_idx)
        if chunk is None:
            print(f"Failed to get chunk {chunk_idx}")
            return False
        
        file_data.extend(chunk)
    
    print(f"Downloaded {len(file_data)} bytes")
    
    # Verify checksum
    computed_hash = hashlib.sha256(file_data).digest()
    print(f"Computed SHA-256: {computed_hash.hex()}")
    
    if computed_hash == metadata['checksum']:
        print("✓ Checksum verification PASSED")
        return True
    else:
        print("✗ Checksum verification FAILED")
        return False

def main():
    """Main test function"""
    print("File Server Test Client")
    print(f"Connecting to {SERVER_HOST}:{SERVER_PORT}")
    
    # Create UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(5.0)  # 5 second timeout
    
    try:
        # Test tokenizer (smaller file)
        success1 = download_file(sock, FILE_TOKENIZER, "tokenizer.bin")
        
        # Test model
        success2 = download_file(sock, FILE_MODEL, "stories15M.bin")
        
        print("\n=== Test Results ===")
        print(f"Tokenizer: {'PASS' if success1 else 'FAIL'}")
        print(f"Model: {'PASS' if success2 else 'FAIL'}")
        
        if success1 and success2:
            print("\n✓ All tests passed!")
        else:
            print("\n✗ Some tests failed")
            
    except socket.timeout:
        print("ERROR: Socket timeout - is the server running?")
    except Exception as e:
        print(f"ERROR: {e}")
    finally:
        sock.close()

if __name__ == '__main__':
    main()
