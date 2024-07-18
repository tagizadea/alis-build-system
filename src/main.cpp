#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <commands.hpp>
#include <operations.hpp>
using namespace std;
namespace fs = filesystem;

const string path = ".";

vector <string> files;

int main(int argc, char *argv[]){

    cout << "Ali's Build System ALPHA!\n";

    try{
        for(const auto& entry : fs::directory_iterator(path)){
            // cout << entry.path().string() << endl;
            files.push_back(entry.path().string());
        }
    }
    catch(const fs::filesystem_error& e){
        cout << "Error: " << e.what() << endl;
        return 0;
    }

    vector <string> SystemFiles = getSystemFiles(files);

    if(argc == 1 && SystemFiles.empty()){
        command::help();
        return 0;
    }

    for(int i=0; i<SystemFiles.size(); i++){
        cout << SystemFiles[i] << '\n';
        string line;
        ifstream file(SystemFiles[i]); 
        while(getline(file, line)){
            cout << line << '\n';
        }
    }

    return 0;
}