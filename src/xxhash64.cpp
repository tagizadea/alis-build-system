#include <xxhash64.hpp>
#include <debug.hpp>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace{
    constexpr uint64_t PRIME1 = 11400714785074694791ULL;
    constexpr uint64_t PRIME2 = 14029467366897019727ULL;
    constexpr uint64_t PRIME3 = 1609587929392839161ULL;
    constexpr uint64_t PRIME4 = 9650029242287828579ULL;
    constexpr uint64_t PRIME5 = 2870177450012600261ULL;

    inline uint64_t rotl(uint64_t x, int r){
        return (x << r) | (x >> (64 - r));
    }

    inline uint64_t round(uint64_t acc, uint64_t input){
        acc += input * PRIME2;
        acc = rotl(acc, 31);
        acc *= PRIME1;
        return acc;
    }

    inline uint64_t merge_round(uint64_t acc, uint64_t val){
        acc ^= round(0, val);
        acc = acc * PRIME1 + PRIME4;
        return acc;
    }

    inline uint64_t avalanche(uint64_t h){
        h ^= h >> 33;
        h *= PRIME2;
        h ^= h >> 29;
        h *= PRIME3;
        h ^= h >> 32;
        return h;
    }

    inline uint64_t read64(const unsigned char* p){
        uint64_t v;
        std::memcpy(&v, p, 8);
        return v;
    }

    // Merges the four lanes after all full 32-byte stripes were consumed.
    inline uint64_t merge_lanes(uint64_t v1, uint64_t v2, uint64_t v3, uint64_t v4){
        uint64_t h = rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18);
        h = merge_round(h, v1);
        h = merge_round(h, v2);
        h = merge_round(h, v3);
        h = merge_round(h, v4);
        return h;
    }

    // Mixes in the last 0-31 bytes and finishes the hash.
    inline uint64_t finalize(uint64_t h, const unsigned char* p, const unsigned char* end){
        while(p + 8 <= end){
            h ^= round(0, read64(p));
            h = rotl(h, 27) * PRIME1 + PRIME4;
            p += 8;
        }

        if(p + 4 <= end){
            uint32_t v;
            std::memcpy(&v, p, 4);
            h ^= (uint64_t)v * PRIME1;
            h = rotl(h, 23) * PRIME2 + PRIME3;
            p += 4;
        }

        while(p < end){
            h ^= (*p) * PRIME5;
            h = rotl(h, 11) * PRIME1;
            ++p;
        }

        return avalanche(h);
    }
}

namespace xxh64{
    uint64_t hash_bytes(const void* data, size_t len, uint64_t seed){
        const unsigned char* p = static_cast<const unsigned char*>(data);
        const unsigned char* end = p + len;

        uint64_t h;

        if(len >= 32){
            const unsigned char* limit = end - 32;

            uint64_t v1 = seed + PRIME1 + PRIME2;
            uint64_t v2 = seed + PRIME2;
            uint64_t v3 = seed;
            uint64_t v4 = seed - PRIME1;

            do{
                v1 = round(v1, read64(p)); p += 8;
                v2 = round(v2, read64(p)); p += 8;
                v3 = round(v3, read64(p)); p += 8;
                v4 = round(v4, read64(p)); p += 8;
            }
            while(p <= limit);

            // p now points at the 0-31 tail bytes, which finalize() mixes in
            h = merge_lanes(v1, v2, v3, v4);
        }
        else h = seed + PRIME5;

        h += len;
        return finalize(h, p, end);
    }

    // String hashing
    uint64_t hash_string(const std::string& s, uint64_t seed){
        return hash_bytes(s.data(), s.size(), seed);
    }

    // File hashing: streaming XXH64, same result as hash_bytes() on the whole file.
    // Stripes that cross a buffer boundary are carried over in `carry`.
    uint64_t hash_file(const std::string& path, uint64_t seed){
        ABS_PROFILE_FUNC();
        std::ifstream f(path, std::ios::binary);
        if (!f)
            throw std::runtime_error("cannot open file");

        std::vector<char> buffer(64 * 1024);

        uint64_t v1 = seed + PRIME1 + PRIME2;
        uint64_t v2 = seed + PRIME2;
        uint64_t v3 = seed;
        uint64_t v4 = seed - PRIME1;

        unsigned char carry[32];
        size_t carried = 0;
        uint64_t total = 0;

        auto stripe = [&](const unsigned char* s){
            v1 = round(v1, read64(s));
            v2 = round(v2, read64(s + 8));
            v3 = round(v3, read64(s + 16));
            v4 = round(v4, read64(s + 24));
        };

        while(f){
            f.read(buffer.data(), buffer.size());
            std::streamsize n = f.gcount();
            if (n <= 0) break;

            total += (uint64_t)n;

            const unsigned char* p = reinterpret_cast<unsigned char*>(buffer.data());
            const unsigned char* end = p + n;

            // Complete a stripe left over from the previous buffer
            if(carried > 0){
                size_t fill = std::min<size_t>(32 - carried, end - p);
                std::memcpy(carry + carried, p, fill);
                carried += fill;
                p += fill;
                if(carried < 32) continue;
                stripe(carry);
                carried = 0;
            }

            while(p + 32 <= end){
                stripe(p);
                p += 32;
            }

            if(p < end){
                carried = end - p;
                std::memcpy(carry, p, carried);
            }
        }

        uint64_t h = (total >= 32) ? merge_lanes(v1, v2, v3, v4) : seed + PRIME5;
        h += total;
        return finalize(h, carry, carry + carried);
    }
}