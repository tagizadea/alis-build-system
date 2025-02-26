#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <commands.hpp>
#include <operations.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <eval.hpp>
using namespace std;
namespace fs = filesystem;

const string path = ".";
const string MainFile = "./main.abs";
bool mainFileFlag = false;

vector <string> files;

// Printing Abstact Syntax Tree using Statements
void print_stmt(Stmt* stmt, int tab){
    NodeType kind = stmt->getKind();
    string tab_s = "";
    for(int i = 0; i < tab; ++i) tab_s += "    ";
    if(NodeType::PROGRAM == kind){
        cout << tab_s << "Type: Program";
        Program* childObj = dynamic_cast<Program*>(stmt);
        cout << tab_s << "{\n";
        for(int i = 0; i < childObj->body.size();++i){
            print_stmt(childObj->body[i], tab + 1);
            cout << '\n';
        }
        cout << tab_s << "}\n";
    }
    else if(NodeType::BINARYEXPR == kind){
        cout << tab_s << "Type: BinaryExpr\n";
        BinaryExpr* childObj = dynamic_cast<BinaryExpr*>(stmt);
        print_stmt(childObj->left, tab + 1);
        cout << tab_s << "Operator: " << childObj->op << '\n';
        print_stmt(childObj->right, tab + 1);
        cout << '\n';
    }
    else if(NodeType::NUMERIC_L == kind){
        cout << tab_s  << "Type: NumbericLiteral\n";
        NumericLiteral* childObj = dynamic_cast<NumericLiteral*>(stmt);
        cout << tab_s  << "Value: " << childObj->val << '\n';
    }
    else if(NodeType::OBJECT_L == kind){
        cout << tab_s << "Type: ObjectLiteral\n";
        ObjectLiteral* childObj = dynamic_cast<ObjectLiteral*>(stmt);
        cout << tab_s << "{\n";
        for(int i=0;i<childObj->properties.size();++i) print_stmt(childObj->properties[i], tab + 1);
        cout << tab_s << "}\n";
    }
    else if(NodeType::PROPERTY_L == kind){
        cout << tab_s << "Type: ProperyLiteral\n";
        PropertyLiteral* childObj = dynamic_cast<PropertyLiteral*>(stmt);
        cout << tab_s << "Key: "<<childObj->key << '\n';
        cout << tab_s << "Value:\n";
        print_stmt(childObj->val, tab + 1);
        cout << '\n';
    }
    else if(NodeType::VAR_D == kind){
        cout << tab_s << "Type: VariableDeclaration\n";
        VarDeclaration* childObj = dynamic_cast<VarDeclaration*>(stmt);
        cout << tab_s << "Is_Const: " << (int)childObj->constant << '\n';
        cout << tab_s << "Variable_Name: " << childObj->identifier << '\n';
        cout << tab_s << "Value:\n";
        print_stmt(childObj->val, tab + 1);
        cout << '\n';
    }
    else if(NodeType::ASSIGNEXPR == kind){
        cout << tab_s << "Type: AssignmentExpr\n";
        AssignExpr* childObj = dynamic_cast<AssignExpr*>(stmt);
        print_stmt(childObj->assignexpr, tab + 1);
        cout << tab_s << "Value:\n";
        print_stmt(childObj->value, tab + 1);
        cout << '\n';
    }
    else if(NodeType::IDENTIFIER == kind){
        cout << tab_s  << "Type: Identifier\n";
        Identifier* childObj = dynamic_cast<Identifier*>(stmt);
        cout << tab_s << "Name: " << childObj->symbol << '\n';
    }
    else{
        cout << tab_s  << "Unknown Statement!\n"; // !!! assert ile deyisdir
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

    // Printing Lexer for debug
    cout << "Lexer:\n";
    for(int i = 0; tokens[i].type != TokenType::EndOfFile; ++i){
        cout << "Token: " << tokens[i].value << " (Type: " << rKeywords[tokens[i].type] << ' '
        << (int)tokens[i].type << ")\n";
    }
    cout << '\n';
    
    Parser* parser = new Parser(tokens);
    Program* program = parser->produceAST();

    // Printing AST for debug
    cout << "AST:\n";
    print_stmt(program, 0);

    // Printing Evalutation for debug
    cout << "\nEVALUATION:\n";
    Env* env = new Env;
    InitNatives(env);
    Value* eval = evaluate(program, env);
    if(eval->getType() == ValueType::Number){
        cout << "Type: Number\n";
        NumberVal* temp = (NumberVal*)eval;
        cout << "Value: " << temp->val << '\n';
    }
    else if(eval->getType() == ValueType::Bool){
        cout << "Type: Bool\n";
        BoolValue* temp = (BoolValue*)eval;
        cout << "Value: " << (int)temp->val << '\n'; 
    }
    else{
        cout << "Type: Null\nValue: Null\n";
    }



    return 0;
}