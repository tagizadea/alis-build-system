#include <operations.hpp>
#include <chrono>
#include <ctime>
#include <queue>

vector <NativeFuncVal*> ListVecNFuncs;

vector<string> getSystemFiles(vector<string> &files){
    vector <string> temp;
    for(int i=0; i<files.size(); ++i){
        string s = files[i];
        if(s.size() > 4){
            if(s[s.size() - 1] == 's' && 
            s[s.size() - 2] == 'b' &&
            s[s.size() - 3] == 'a' &&
            s[s.size() - 4] == '.'){
                temp.push_back(s);
            }
        }
    }

    return temp;
}

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
    else if(NodeType::FUN_D == kind){
        cout << tab_s << "Type: FunctionDeclaration\n";
        FunDeclaration* childObj = dynamic_cast<FunDeclaration*>(stmt);
        cout << tab_s << "Name: " << childObj->name << '\n';
        cout << tab_s << "Params: ";
        for(string &s : childObj->parameters) cout << s << ' ';
        cout << '\n' << tab_s << "Body:\n";
        for(Stmt* i : childObj->body){
            print_stmt(i, tab + 1);
        }
        cout << '\n';
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
    else if(NodeType::LIST_L == kind){
        cout << tab_s << "Type: ListLiteral{\n";
        ListLiteral* childObj = dynamic_cast<ListLiteral*>(stmt);
        for(int i=0;i<childObj->properties.size();++i) print_stmt(childObj->properties[i], tab + 1);
    }
    else if(NodeType::ELEMENT_L == kind){
        cout << tab_s << "Type: ElementLiteral\n";
        ElementLiteral* childObj = dynamic_cast<ElementLiteral*>(stmt);
        cout << tab_s << "Index: " << childObj->key << '\n';
        cout << tab_s << "Value:\n";
        if(childObj->val == nullptr) cout << tab_s << "Undefined value!";
        else print_stmt(childObj->val, tab + 1);
        cout << '\n';
    }
    else if(NodeType::PROPERTY_L == kind){
        cout << tab_s << "Type: PropertyLiteral\n";
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
        //cout << tab_s << "ObjectType: " << (int)childObj->object->getKind() << '\n';
        cout << tab_s << "ObjectValue:\n";
        if(childObj->object == nullptr) cout << tab_s << "Undefined value";
        else print_stmt(childObj->object, tab + 1);
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
    if(eval == nullptr){
        cout << tab_s << "NullPTR!\n";
        return;
    }
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
    else if(eval->getType() == ValueType::FUNC){
        cout << tab_s << "Type: Function\n";
        FunctionVal* temp = (FunctionVal*)eval;
        cout << tab_s << "Name: " << temp->name << '\n';
        cout << tab_s << "Params: ";
        for(string &s : temp->params) cout << s << ' ';
        cout << '\n';
    }
    else if(eval->getType() == ValueType::Null){
        cout << tab_s << "Type: Null\n"<< tab_s <<"Value: Null\n";
    }
    else{
        cout << tab_s << "Type: None\n";
    }
}

void print_env(Env* env, int tab){
    if(env->parent == nullptr) cout << "Env: root\n\n";
    else cout << "Env: child\n\n";

    cout << "Variables:\n";
    for(pair <string, Value*> i : env->variables){
        cout << "Name: " << i.first << '\n' << "Value:\n";
        print_eval(i.second, tab + 1);
        cout << '\n';
    }

    cout << "Constants:\n";
    for(string i : env->constants){
        cout << "Name: " << i << '\n';
    }
}

/* ---------------------- ABS OPERATIONS ----------------------*/

Value* n_funs::vector_size(vector<Value*> args, Env* env){
    if(args.size() != 1){
        return env->lookUpVar("Null");
    }
    ListValue* l = (ListValue*)args[0];
    return Make_Number(l->v.size());
}

Value* n_funs::vector_push(vector<Value*> args, Env* env){
    if(args.size() < 2){
        return env->lookUpVar("Null");
    }

    ListValue* l = (ListValue*)args[args.size() - 1];

    if(l->v.empty()) for(int i=0;i<10;++i) l->mapTypeCounter[i] = 0;

    for(int i = 0; i < args.size() - 1; ++i){
        l->v.push_back(args[i]);

        int val_id = (int)args[i]->getType();
        if(l->mapTypeCounter[val_id] == 0) ++l->distinc_types;
        ++l->mapTypeCounter[val_id];
        if(l->distinc_types == 1) l->consist_of = args[i]->getType();
        else l->consist_of = ValueType::None;
    }
    return env->lookUpVar("Null");
}

Value* n_funs::vector_pop(vector<Value*> args, Env* env){
    if(args.size() != 1) return env->lookUpVar("Null");

    ListValue* l = (ListValue*)args[0];
    if(l->v.empty()){
        cout << "List boşdur: pop icra oluna bilmir!";
        exit(0); // !!! debug sistemi ile deyis
    }

    int val_id = (int)l->v[l->v.size() - 1]->getType();
    --l->mapTypeCounter[val_id];
    if(l->mapTypeCounter[val_id] == 0) --l->distinc_types;
    if(l->distinc_types == 1) l->consist_of = l->v[l->v.size() - 1]->getType();
    else l->consist_of = ValueType::None;

    l->v.pop_back();
    return env->lookUpVar("Null");
}

Value* n_funs::vector_sort(vector<Value*> args, Env* env){
    if(args.size() != 1) return env->lookUpVar("Null");

    ListValue* l = (ListValue*)args[0];

    if(l->consist_of == ValueType::Number)
        std::sort(l->v.begin(), l->v.end(), sort_comps::cmp_less_Number);
    else if(l->consist_of == ValueType::String)
        std::sort(l->v.begin(), l->v.end(), sort_comps::cmp_less_String);
    else{
        cout << "Warning: List consist of more than one type!\n"; // Debug sistemi ile evezle
    }
    return env->lookUpVar("Null");
}

Value* n_funs::print(vector<Value*> args, Env* env){ // naive print fun
    // OBJ VE LIST PROBLEMLIDI !!!
    queue <pair <vector <Value*> , string> > q;
    q.push({args, ""});
    while(!q.empty()){
        vector <Value*> v = q.front().first;
        string tab_s = q.front().second;

        for(int i=0;i<v.size();++i){
            if(v[i]->getType() == ValueType::Object){
                ObjectValue* temp = (ObjectValue*)v[i];
                cout << tab_s << "Object\n";
    
                for(pair <string, Value*> j : temp->properties){
                    // cout << tab_s + "    "<< "Key: \"" <<j.first << "\" Value: ";
                    q.push({{j.second}, tab_s + "    " + "Key: \"" + j.first + "\" Value: \n"});
                }
                // cout << '\n';
            }
            else if(v[i]->getType() == ValueType::List){
                ListValue* temp = (ListValue*)v[i];
                cout << tab_s << "List";
    
                for(int j = 0; j < temp->v.size(); ++j){
                    if(j == 0) q.push({{temp->v[j]}, tab_s + "    "});
                    else q.push({{temp->v[j]}, tab_s + " "});
                }
                cout << "\n";
            }
            else if(v[i]->getType() == ValueType::Number){
                NumberVal* temp = (NumberVal*)v[i];
                cout << tab_s << temp->val;
            }
            else if(v[i]->getType() == ValueType::Bool){
                BoolValue* temp = (BoolValue*)v[i];
                if(temp->val) cout << tab_s << "True";
                else cout << tab_s << "False";
            }
            else if(v[i]->getType() == ValueType::String){
                StringVal* temp = (StringVal*)v[i];
                cout << tab_s << temp->val;
            }
            else if(v[i]->getType() == ValueType::Null){
                NullVal* temp = (NullVal*)v[i];
                cout << tab_s << temp->val;
            }
        }

        q.pop();
    }
    //cout << '\n';
    return env->lookUpVar("Null");
}

Value *n_funs::timeNow(vector<Value*> args, Env* env){
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_c);

    // Extract hour and minute
    int hour = local_time->tm_hour;
    int minute = local_time->tm_min;
    return Make_Number(hour * 60 + minute);
}

Value* n_funs::floor(vector<Value*> args, Env* env){
    if(args.size() > 1 || args[0]->getType() != ValueType::Number){
        cout << "Floor Function: Wrong args";
        exit(0); // !!! debug systemi ile deyis
    }
    NumberVal* temp = (NumberVal*)args[0];
    long long res = temp->val;
    return Make_Number(res);
}

Value* n_funs::system(vector<Value*> args, Env* env){
    if(args.size() > 1 || args[0]->getType() != ValueType::String){
        cout << "System Function: Wrong args. Use platform specific commands";
        exit(0); // !!! debug systemi ile deyis
    }
    
    StringVal* temp = (StringVal*)args[0];
    std::system(temp->val.c_str());
    return env->lookUpVar("Null");
}

Value* n_funs::Ntrack(vector<Value*> args, Env* env){
    ListValue* l = new ListValue;
    vector <string> src;
    for(int i = 0; i < args.size(); ++i){
        if(args[i]->getType() != ValueType::String){
            cout << "Track Function: only strings can be argument!";
            exit(0); // !!! debug systemi ile deyis
        }
        string name = ((StringVal*)args[i])->val;
        Manager::getInstance().sources.push_back(name);
    }
    src = track();
    for(string s : src) l->v.push_back(Make_String(s));
    l->consist_of = ValueType::String;
    l->distinc_types = 1;
    l->mapTypeCounter[(int)ValueType::String] = src.size();
    return l;
}

Value* n_funs::compile(vector<Value*> args, Env* env){
    if(args.size() != 2){
        cout << "Compile Function: Number of args must be 2 (compiler and source files)";
        exit(0); // !!! debug sistemi ile deyis
    }

    return nullptr; // Not implemented yet
}

Value* n_funs::run(vector<Value*> args, Env* env){
    if(args.size() != 1){
        cout << "Run Function: Too many arguments!";
        exit(0); // !!! debug sistemi ile deyis
    }
    if(args[0]->getType() != ValueType::String){
        cout << "Run Function: Argument should be String!";
        exit(0); // !!! debug sistemi ile deyis
    }
    
    // ObjectValue* o = new ObjectValue;

    ifstream file(((StringVal*)args[0])->val);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    content += '\n';
    Lexer lexer(content.c_str());
    Token* tokens = lexer.tokenize();

    Parser* parser = new Parser(tokens);
    Program* program = parser->produceAST();

    // Env* local_env = new Env;
    // InitNatives(local_env);
    Value* eval = evaluate(program, env);

    return env->lookUpVar("Null");
}

/* SORT COMPARATORS FOR DEFAULT TYPES*/

bool sort_comps::cmp_less_Number(Value* a, Value* b){
    return ((NumberVal*)a)->val < ((NumberVal*)b)->val;
}

bool sort_comps::cmp_less_String(Value* a, Value* b){
    return ((StringVal*)a)->val < ((StringVal*)b)->val;
}
