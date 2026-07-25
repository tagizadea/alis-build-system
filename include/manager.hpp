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

using namespace std;
namespace fs = std::filesystem;

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

class Manager{
    private:

    Manager() = default;
    ~Manager() = default;

    struct FileCacheEntry{
        string name;
        uint64_t size;
        uint64_t mtime;
        uint64_t content_hash;
        // uint64_t dependency_hash;

        void save(ostream &os) const{
            writeBinary(os, name);
            writeBinary(os, size);
            writeBinary(os, mtime);
            writeBinary(os, content_hash);
        }

        void load(istream &is){
            readBinary(is, name);
            readBinary(is, size);
            readBinary(is, mtime);
            readBinary(is, content_hash);
        }

    };

    struct DependencyCacheEntry{
        string name;
        vector <string> headers;

        void save(ostream &os) const{
            writeBinary(os, name);
            writeBinary(os, headers);
        }

        void load(istream &is){
            readBinary(is, name);
            readBinary(is, headers);
        }
    };

    struct ObjectCacheEntry{
        string name;
        uint64_t build_hash;
        uint64_t object_size;

        void save(ostream &os) const{
            writeBinary(os, name);
            writeBinary(os, build_hash);
            writeBinary(os, object_size);
        }

        void load(istream &is){
            readBinary(is, name);
            readBinary(is, build_hash);
            readBinary(is, object_size);
        }
    };

    public:
    
    static Manager &getInstance(){
        static Manager instance;
        return instance;
    }

    // OLD !!!
    vector <string> sources;
    unordered_map <string, pair <unsigned int, uint32_t>> tracked_files;
    map <string, string> deps;
    map <string, optional<string>> defines;

    // Caches
    map <string, FileCacheEntry> FileCache;
    map <string, DependencyCacheEntry> DependencyCache;
    map <string, ObjectCacheEntry> ObjectCache;

    void init_manager();
};

vector <string> track();
vector <string> scan_headers();

#endif