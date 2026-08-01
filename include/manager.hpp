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

/// Returns the file size in bytes.
/// Returns 0 if the file does not exist or cannot be accessed.
inline uint64_t GetFileSize(const fs::path& path){
    error_code ec;
    auto size = fs::file_size(path, ec);
    return ec ? 0 : static_cast<uint64_t>(size);
}

/// Returns the file modification time as nanoseconds since Unix epoch.
/// Returns 0 if the file does not exist or cannot be accessed.
inline uint64_t GetFileMTime(const fs::path& path){
    error_code ec;
    auto fileTime = fs::last_write_time(path, ec);
    if(ec) return 0;
    auto systemTime = fs::file_time_type::clock::to_sys(fileTime);
    auto ns = duration_cast<chrono::nanoseconds>( systemTime.time_since_epoch() );
    return static_cast<uint64_t>(ns.count());
}

template<typename T>
typename enable_if<is_trivially_copyable<T>::value, void>::type
writeBinary(ostream& os, const T& value){
    os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
typename enable_if<is_trivially_copyable<T>::value, void>::type
readBinary(istream &is, T &value){
    is.read(reinterpret_cast<char*>(&value), sizeof(T));
}

inline void writeBinary(ostream &os, const string &s){
    size_t size = s.size();
    writeBinary(os, size);
    if(size > 0) os.write(s.data(), size);
}

inline void readBinary(istream &is, string &s){
    size_t size;
    readBinary(is, size);
    s.resize(size);
    if(size > 0) is.read(&s[0], size);
}

template<typename T>
void writeBinary(ostream &os, const vector<T> &vec){
    size_t size = vec.size();
    writeBinary(os, size);
    if constexpr(is_trivially_copyable<T>::value){
        if(size > 0) os.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(T));
    }
    else for(const auto &item : vec) writeBinary(os, item);
}

template<typename T>
void readBinary(istream &is, vector<T> &vec){
    size_t size;
    readBinary(is, size);
    vec.resize(size);
    if(size > 0){
        if constexpr(is_trivially_copyable<T>::value) is.read(reinterpret_cast<char*>(vec.data()), size * sizeof(T));
        else for(auto &item : vec) readBinary(is, item);
    }
}

const string DEPS_CACHE_FILE_NAME = "dependencies.cache";
const string FILES_CACHE_FILE_NAME = "files.cache";
const string OBJS_CACHE_FILE_NAME = "objects.cache";

template<typename Entry, typename Cache>
void readCache(const string& filename, Cache& cache){
    const string path = ".abs/" + filename;

    if(!fs::exists(path)){
        {
            fstream create(path, ios::out | ios::binary);
            size_t size = 0;
            writeBinary(create, size);
        }
    }

    fstream temp_fstr(path, ios::in | ios::binary);

    if(!temp_fstr.is_open()){
        cout << "Manager error: The file \"" << filename << "\" could not be opened!";
        exit(0);
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
void writeCache(const string& filename, const Cache& cache){
    const string path = ".abs/" + filename;

    fstream temp_fstr(path, ios::out | ios::binary | ios::trunc);

    if(!temp_fstr.is_open()){
        cout << "Manager error: The file \"" << filename << "\" could not be opened!";
        exit(0);
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
        string name;
        uint64_t size;
        uint64_t mtime;
        uint64_t content_hash;

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
        vector <string> includes;
        uint64_t dependency_hash;

        void save(ostream &os) const{
            writeBinary(os, name);
            writeBinary(os, includes);
            writeBinary(os, dependency_hash);
        }

        void load(istream &is){
            readBinary(is, name);
            readBinary(is, includes);
            readBinary(is, dependency_hash);
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
    
    static Manager &getInstance(){
        static Manager instance;
        return instance;
    }

    // OLD !!!
    vector <string> sources;
    unordered_map <string, pair <unsigned int, uint32_t>> tracked_files;
    map <string, string> deps;

    map <string, optional<string>> defines;
    vector <string> include_paths;

    // Caches
    map <string, FileCacheEntry> FileCache;
    map <string, DependencyCacheEntry> DependencyCache;
    map <string, ObjectCacheEntry> ObjectCache;



    void init_manager();
    
    FileCacheEntry track(const string& Source);
    vector <string> scan_headers(const string& ChangedFile);
    vector <string> scan_graph(const DependencyCacheEntry& Node, set <string>& color, map <string, vector <string>>& reverse_graph);
    vector <string> reverse_invalidation(map <string, vector <string>>& reverse_graph, vector <string>& DirtyHeaderNames);
    
};

#endif