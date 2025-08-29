#ifndef MANAGER
#define MANAGER

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

using namespace std;
namespace fs = std::filesystem;

class Manager{
    private:

    Manager() = default;
    ~Manager() = default;

    public:
    
    static Manager& getInstance(){
        static Manager instance;
        return instance;
    }
    vector <string> sources;
    unordered_map <string, pair <unsigned int, uint32_t>> tracked_files, current_files;
};

void init_manager();
vector <string> track();


#endif