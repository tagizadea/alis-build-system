#ifndef XXHASH64
#define XXHASH64

#include <cstddef>
#include <cstdint>
#include <string>

namespace xxh64{
    uint64_t hash_bytes(const void* data, size_t len, uint64_t seed = 0);
    uint64_t hash_string(const std::string& s, uint64_t seed = 0);
    uint64_t hash_file(const std::string& path, uint64_t seed = 0);
}
#endif