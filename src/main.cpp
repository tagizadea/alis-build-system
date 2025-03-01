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
    else if(NodeType::CONDEXPR == kind){
        cout << tab_s << "Type: ConditionExpr\n";
        CondExpr* childObj = dynamic_cast<CondExpr*>(stmt);
        cout << tab_s << "Condition:\n";
        print_stmt(childObj->condition, tab + 1);
        cout << tab_s << "Then{\n";
        for(Stmt* i : childObj->ThenBranch){
            print_stmt(i, tab + 1);
        }
        cout << tab_s << "}\n" << tab_s << "Else{\n";
        if(childObj->ElseBranch == nullptr) cout << tab_s << "No ElseBranch\n";
        else print_stmt(childObj->ElseBranch, tab + 1);
        cout << tab_s << "}\n";
    }
    else if(NodeType::WHILE_LOOP == kind){
        cout << tab_s << "Type: WHILE\n";
        WhileStmt* childObj = dynamic_cast<WhileStmt*>(stmt);
        cout << tab_s << "Condition:\n";
        print_stmt(childObj->condition, tab + 1);
        cout << tab_s << "Then{\n";
        for(Stmt* i : childObj->ThenBranch){
            print_stmt(i, tab + 1);
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
    else if(NodeType::NOTEXPR == kind){
        cout << tab_s << "Type: NotExpr\n";
        NotExpr* childObj = dynamic_cast<NotExpr*>(stmt);
        cout << tab_s << "Value:\n";
        print_stmt(childObj->val, tab + 1);
        cout << '\n';
    }
    else if(NodeType::NUMERIC_L == kind){
        cout << tab_s  << "Type: NumericLiteral\n";
        NumericLiteral* childObj = dynamic_cast<NumericLiteral*>(stmt);
        cout << tab_s  << "Value: " << childObj->val << '\n';
    }
    else if(NodeType::STRING_L == kind){
        cout << tab_s << "Type: StringLiteral\n";
        StringLiteral* childObj = dynamic_cast<StringLiteral*>(stmt);
        cout << tab_s << "Value: " << childObj->val << '\n';
    }
    else if(NodeType::OBJECT_L == kind){
        cout << tab_s << "Type: ObjectLiteral";
        ObjectLiteral* childObj = dynamic_cast<ObjectLiteral*>(stmt);
        cout << "{\n";
        for(int i=0;i<childObj->properties.size();++i) print_stmt(childObj->properties[i], tab + 1);
        cout << tab_s << "}\n";
    }
    else if(NodeType::PROPERTY_L == kind){
        cout << tab_s << "Type: ProperyLiteral\n";
        PropertyLiteral* childObj = dynamic_cast<PropertyLiteral*>(stmt);
        cout << tab_s << "Key: "<<childObj->key << '\n';
        cout << tab_s << "Value:\n";
        if(childObj->val == nullptr) cout << tab_s << "Undefined value!";
        else print_stmt(childObj->val, tab + 1);
        cout << '\n';
    }
    else if(NodeType::VAR_D == kind){
        cout << tab_s << "Type: VariableDeclaration\n";
        VarDeclaration* childObj = dynamic_cast<VarDeclaration*>(stmt);
        cout << tab_s << "Is_Const: " << (int)childObj->constant << '\n';
        cout << tab_s << "Variable_Name: " << childObj->identifier << '\n';
        cout << tab_s << "Value:\n";
        if(childObj->val == nullptr) cout << tab_s << "Undefined value";
        else print_stmt(childObj->val, tab + 1);
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
    else if(NodeType::MEMBEREXPR == kind){
        cout << tab_s << "Type: MemberExpr\n";
        MemberExpr* childObj = dynamic_cast<MemberExpr*>(stmt);
        cout << tab_s << "IsComputed: " << (int)childObj->computed << '\n';
        //cout << "Object:\n";
        //if(childObj->object == nullptr) cout << tab_s << "Undefined value";
        //else print_stmt(childObj->object, tab + 1);
        cout << tab_s << "Property:\n";
        if(childObj->property == nullptr) cout << tab_s << "Undefined value";
        else print_stmt(childObj->property, tab + 1);
        cout << '\n';
    }
    else if(NodeType::CALLEXPR == kind){
        cout << tab_s << "Type: CallExpr\n";
        CallExpr* childObj = dynamic_cast<CallExpr*>(stmt);
        cout << tab_s << "Args:{\n";
        for(int i=0; i < childObj->args.size();++i){
            print_stmt(childObj->args[i], tab + 1);
        }
        cout << tab_s << "}\n";
        cout << tab_s << "CallEr:\n";
        print_stmt(childObj->callexpr, tab + 1);
        cout << '\n';
    }
    else{
        cout << tab_s  << "Unknown Statement!\n"; // !!! assert ile deyisdir
    }
}

// Printing Evaluation
void print_eval(Value* eval, int tab){
    string tab_s = "";
    for(int i = 0; i < tab; ++i) tab_s += "    ";
    if(eval->getType() == ValueType::Number){
        cout << tab_s << "Type: Number\n";
        NumberVal* temp = (NumberVal*)eval;
        cout << tab_s << "Value: " << temp->val << '\n';
    }
    else if(eval->getType() == ValueType::String){
        cout << tab_s << "Type: String\n";
        StringVal* temp = (StringVal*)eval;
        cout << tab_s << "Value: " << temp->val << '\n';
    }
    else if(eval->getType() == ValueType::Bool){
        cout << tab_s << "Type: Bool\n";
        BoolValue* temp = (BoolValue*)eval;
        cout << tab_s << "Value: " << (int)temp->val << '\n'; 
    }
    else if(eval->getType() == ValueType::Object){
        cout << tab_s << "Type: Object\n";
        ObjectValue* temp = (ObjectValue*)eval;
        for(pair <string, Value*> i : temp->properties){
            cout << tab_s << "Key: " << i.first << '\n';
            cout << tab_s << "Value:\n";
            if(i.second == nullptr) cout << tab_s << "Unknown value!";
            else print_eval(i.second, tab + 1);
            cout << '\n';
        }
    }
    else if(eval->getType() == ValueType::NFUNC){
        cout << tab_s << "Type: NativeFunc\n";
        //NativeFuncVal* temp = (NativeFuncVal*)eval;
    }
    else if(eval->getType() == ValueType::Null){
        cout << tab_s << "Type: Null\n"<< tab_s <<"Value: Null\n";
    }
    else{
        cout << tab_s << "Type: None\n";
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
    Env* env = new Env;
    InitNatives(env);
    Value* eval = evaluate(program, env);
    //cout << "\nEVALUATION:\n";
    //print_eval(eval, 0);
    return 0;
}