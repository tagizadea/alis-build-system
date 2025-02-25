#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <commands.hpp>
#include <operations.hpp>
#include <lexer.hpp>
#include <parser.hpp>
using namespace std;
namespace fs = filesystem;

const string path = ".";
const string MainFile = "./main.abs";
bool mainFileFlag = false;

vector <string> files;


void print_stmt(Stmt* stmt){
    //NodeType kind = stmt->kind;
    NodeType kind = stmt->getKind();
    if(NodeType::PROGRAM == kind){
        cout << "Type: Program\n";
        Program* childObj = dynamic_cast<Program*>(stmt);
        for(int i = 0; i < childObj->body.size();++i){
            print_stmt(childObj->body[i]);
        }

    }
    else if(NodeType::BINARYEXPR == kind){
        cout << "Type: BinaryExpr\n";
        BinaryExpr* childObj = dynamic_cast<BinaryExpr*>(stmt);
        print_stmt(childObj->left);
        cout << '\n';
        cout << childObj->op << '\n';
        print_stmt(childObj->right);
        cout << '\n';
    }
    else if(NodeType::NUMERIC_L == kind){
        cout << "Type: NumbericLiteral\n";
        NumericLiteral* childObj = dynamic_cast<NumericLiteral*>(stmt);
        cout << "Value: " << childObj->val << '\n';
    }
    else if(NodeType::IDENTIFIER == kind){
        cout << "Type: Identifier\n";
        Identifier* childObj = dynamic_cast<Identifier*>(stmt);
        cout << childObj->symbol << '\n';
    }
    else{
        cout << "Unknown Statement!\n"; // !!! assert ile deyisdir
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

    ifstream file(MainFile);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    Lexer lexer(content.c_str());
    Token* tokens = lexer.tokenize();

    for(int i = 0; tokens[i].type != TokenType::EndOfFile; ++i){
        cout << "Token: " << tokens[i].value << " (Type: " << rKeywords[tokens[i].type] << ' '
        << (int)tokens[i].type << ")\n";
    }

    cout << '\n';

    Parser* parser = new Parser(tokens);

    Program* program = parser->produceAST();


    print_stmt(program);

    return 0;
}