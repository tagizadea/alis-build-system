#ifndef MANAGER
#define MANAGER

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>

namespace fs = std::filesystem;

class Manager{
    private:

    Manager() = default;
    ~Manager() = default;

    public:
    
    Manager& getInstance(){
        static Manager instance;
        return instance;
    }

    
};

void init_manager();


#endif