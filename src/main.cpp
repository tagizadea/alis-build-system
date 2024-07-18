#include <iostream>
#include <fstream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

const string path = ".";

int main(){
    try{
        for(const auto& entry : fs::directory_iterator(path)){
            cout << entry.path().string() << endl;
        }
    }
    catch(const fs::filesystem_error& e){
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}