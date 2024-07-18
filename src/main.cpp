#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <commands.hpp>
#include <operations.hpp>
using namespace std;
namespace fs = filesystem;

const string path = ".";
const string MainFile = "./main.abs";
bool mainFileFlag = false;

vector <string> files;

void execute(string s){
    if(functionMap.find(s) != functionMap.end()){
        functionMap[s]();
    }
    else{
        cout << "BELE BIR FUNKSIYA TAPILMADI!\n";
    }
}

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
        //cout << SystemFiles[i] << '\n';
        if(SystemFiles[i] == MainFile) mainFileFlag = true;
        //string line;
        //ifstream file(SystemFiles[i]); 
        //while(getline(file, line)){
        //    cout << line << '\n';
        //}
    }

    cout << endl; //

    if(!mainFileFlag){
        cout << "Xəta: Main faylı tapılmadı\n";
        return 0;
    }

    string line;
    ifstream file(MainFile); 
    while(getline(file, line)){
        string s;
        for(int i=0; i<line.size(); i++){
            if(line[i] == ' ' or i == line.size() - 1){
                if(i == line.size() - 1) s += line[i];
                cout << s << '\n';
                execute(s);
                s = "";
            }
            else{
                s += line[i];
            }
        }
    }

    return 0;
}