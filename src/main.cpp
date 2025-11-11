#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <operations.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <eval.hpp>
#include <manager.hpp>
using namespace std;
namespace fs = filesystem;

const string path = ".";
string MainFile = "./main.abs";
bool mainFileFlag = false;
vector <string> files;

int main(int argc, char* argv[]){

    cout << "Ali's Build System ALPHA!\n";
    init_manager();

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

    if(argc > 2){
        cout << "ABS can only run one script!";
        return 0;
    }

    for(int i=0; i<SystemFiles.size(); i++){
        if(SystemFiles[i] == MainFile) mainFileFlag = true;
        if(argc == 2 && SystemFiles[i] == argv[1]) mainFileFlag = true;
    }

    cout << endl; //

    if(!mainFileFlag){
        cout << "Error: Build Script not found!\n";
        return 0;
    }

    if(argc == 2) MainFile = argv[1];

    ifstream file(MainFile);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    content += '\n';
    Lexer lexer(content.c_str());
    Token* tokens = lexer.tokenize();

    // Printing Lexer for debug
    /*cout << "Lexer:\n";
    for(int i = 0; tokens[i].type != TokenType::EndOfFile; ++i){
        cout << "Token: " << tokens[i].value << " (Type: " << rKeywords[tokens[i].type] << ' '
        << (int)tokens[i].type << ")\n";
    }
    cout << '\n';*/
    
    Parser* parser = new Parser(tokens);
    Program* program = parser->produceAST();

    // Printing AST for debug
    cout << "AST:\n";
    print_stmt(program, 0);

    // Printing Evalutation for debug
    Env* env = new Env;
    InitNatives(env);
    Value* eval = evaluate(program, env);
    //cout << "\nEVALUATION:\n";
    //print_eval(eval, 0);

    return 0;
}