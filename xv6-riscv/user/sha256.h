// user/sha256.h
// SHA-256 cryptographic hash function interface for xv6

#ifndef SHA256_H
#define SHA256_H

// Compute SHA-256 hash of input data
// Parameters:
//   data: pointer to input data buffer
//   len: length of input data in bytes
//   hash: output buffer for 32-byte (256-bit) hash result
void sha256_hash(const unsigned char *data, unsigned int len, unsigned char hash[32]);

#endif // SHA256_H
