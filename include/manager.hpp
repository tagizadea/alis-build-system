#ifndef MANAGER
#define MANAGER

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <optional>
#include <any>
#include <type_traits>
#include <debug.hpp>
#include <portable.hpp>

namespace fs = std::filesystem;

/// Returns the file size in bytes.
/// Returns 0 if the file does not exist or cannot be accessed.
inline uint64_t GetFileSize(const fs::path& path){
    std::error_code ec;
    auto size = fs::file_size(path, ec);
    return ec ? 0 : static_cast<uint64_t>(size);
}

/// Returns the file modification time as nanoseconds since Unix epoch.
/// Returns 0 if the file does not exist or cannot be accessed.
inline uint64_t GetFileMTime(const fs::path& path){
    std::error_code ec;
    auto fileTime = fs::last_write_time(path, ec);
    if(ec) return 0;
    auto systemTime = fs::file_time_type::clock::to_sys(fileTime);
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>( systemTime.time_since_epoch() );
    return static_cast<uint64_t>(ns.count());
}

template<typename T>
typename std::enable_if<std::is_trivially_copyable<T>::value, void>::type
writeBinary(std::ostream& os, const T& value){
    os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
typename std::enable_if<std::is_trivially_copyable<T>::value, void>::type
readBinary(std::istream &is, T &value){
    is.read(reinterpret_cast<char*>(&value), sizeof(T));
}

// On 32-bit platforms, split uint64_t into two uint32_t writes/reads to
// avoid 64-bit software emulation. On-disk format stays 8 bytes.
#if ABS_32BIT
inline void writeBinary(std::ostream& os, uint64_t value){
    uint32_t hi = static_cast<uint32_t>(value >> 32);
    uint32_t lo = static_cast<uint32_t>(value & 0xFFFFFFFFULL);
    writeBinary(os, hi);
    writeBinary(os, lo);
}

inline void readBinary(std::istream& is, uint64_t& value){
    uint32_t hi = 0, lo = 0;
    readBinary(is, hi);
    readBinary(is, lo);
    value = (static_cast<uint64_t>(hi) << 32) | lo;
}
#endif

inline void writeBinary(std::ostream &os, const std::string &s){
    size_t size = s.size();
    writeBinary(os, size);
    if(size > 0) os.write(s.data(), size);
}

inline void readBinary(std::istream &is, std::string &s){
    size_t size;
    readBinary(is, size);
    s.resize(size);
    if(size > 0) is.read(&s[0], size);
}

template<typename T>
void writeBinary(std::ostream &os, const std::vector<T> &vec){
    size_t size = vec.size();
    writeBinary(os, size);
    if constexpr(std::is_trivially_copyable<T>::value){
        if(size > 0) os.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(T));
    }
    else for(const auto &item : vec) writeBinary(os, item);
}

template<typename T>
void readBinary(std::istream &is, std::vector<T> &vec){
    size_t size;
    readBinary(is, size);
    vec.resize(size);
    if(size > 0){
        if constexpr(std::is_trivially_copyable<T>::value) is.read(reinterpret_cast<char*>(vec.data()), size * sizeof(T));
        else for(auto &item : vec) readBinary(is, item);
    }
}

const std::string DEPS_CACHE_FILE_NAME = "dependencies.cache";
const std::string FILES_CACHE_FILE_NAME = "files.cache";
const std::string OBJS_CACHE_FILE_NAME = "objects.cache";

template<typename Entry, typename Cache>
void readCache(const std::string& filename, Cache& cache){
    const std::string path = ".abs/" + filename;

    if(!fs::exists(path)){
        {
            std::fstream create(path, std::ios::out | std::ios::binary);
            size_t size = 0;
            writeBinary(create, size);
        }
    }

    std::fstream temp_fstr(path, std::ios::in | std::ios::binary);

    if(!temp_fstr.is_open()){
        ABS_FATAL(cat::Manager, "manager.cache_open_fail", filename);
    }

    size_t size = 0;
    readBinary(temp_fstr, size);

    for(size_t i = 0; i < size; ++i){
        Entry temp;
        temp.load(temp_fstr);
        cache.insert({temp.name, temp});
    }
}

template<typename Entry, typename Cache>
void writeCache(const std::string& filename, const Cache& cache){
    const std::string path = ".abs/" + filename;

    std::fstream temp_fstr(path, std::ios::out | std::ios::binary | std::ios::trunc);

    if(!temp_fstr.is_open()){
        ABS_FATAL(cat::Manager, "manager.cache_open_fail", filename);
    }

    size_t size = cache.size();
    writeBinary(temp_fstr, size);

    for(const auto& pair : cache){
        const Entry& temp = pair.second;
        temp.save(temp_fstr);
    }

    temp_fstr.close();
}

class Manager{
    private:

    Manager() = default;
    ~Manager() = default;

    public:

    struct FileCacheEntry{
        std::string name;
        uint64_t size;
        uint64_t mtime;
        uint64_t content_hash;

        void save(std::ostream &os) const{
            writeBinary(os, name);
            writeBinary(os, size);
            writeBinary(os, mtime);
            writeBinary(os, content_hash);
        }

        void load(std::istream &is){
            readBinary(is, name);
            readBinary(is, size);
            readBinary(is, mtime);
            readBinary(is, content_hash);
        }

    };

    struct DependencyCacheEntry{
        std::string name;
        std::vector <std::string> includes;
        uint64_t dependency_hash;

        void save(std::ostream &os) const{
            writeBinary(os, name);
            writeBinary(os, includes);
            writeBinary(os, dependency_hash);
        }

        void load(std::istream &is){
            readBinary(is, name);
            readBinary(is, includes);
            readBinary(is, dependency_hash);
        }
    };

    struct ObjectCacheEntry{
        std::string name;
        uint64_t build_hash;
        uint64_t object_size;

        void save(std::ostream &os) const{
            writeBinary(os, name);
            writeBinary(os, build_hash);
            writeBinary(os, object_size);
        }

        void load(std::istream &is){
            readBinary(is, name);
            readBinary(is, build_hash);
            readBinary(is, object_size);
        }
    };
    
    static Manager &getInstance(){
        static Manager instance;
        return instance;
    }

    // OLD !!!
    std::vector <std::string> sources;
    std::unordered_map <std::string, std::pair <unsigned int, uint32_t>> tracked_files;
    std::map <std::string, std::string> deps;

    std::map <std::string, std::optional<std::string>> defines;
    std::vector <std::string> include_paths;

    // Caches
    std::map <std::string, FileCacheEntry> FileCache;
    std::map <std::string, DependencyCacheEntry> DependencyCache;
    std::map <std::string, ObjectCacheEntry> ObjectCache;



    void init_manager();
    
    FileCacheEntry track(const std::string& Source);
    std::vector <std::string> scan_headers(const std::string& ChangedFile);
    std::vector <std::string> scan_graph(const DependencyCacheEntry& Node, std::set <std::string>& color, std::map <std::string, std::vector <std::string>>& reverse_graph);
    std::vector <std::string> reverse_invalidation(std::map <std::string, std::vector <std::string>>& reverse_graph, std::vector <std::string>& DirtyHeaderNames);
};

#endif