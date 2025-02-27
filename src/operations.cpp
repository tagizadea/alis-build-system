#include <operations.hpp>
#include <chrono>
#include <ctime>

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

/* ---------------------- ABS OPERATIONS ----------------------*/



Value* n_funs::print(vector <Value*> args, Env* env){ // naive print fun | string ve endl duzelt
    for(int i=0;i<args.size();++i){
        if(args[i]->getType() == ValueType::Object){
            ObjectValue* temp = (ObjectValue*)args[i];
            cout << "Object{\n";

            for(pair <string, Value*> i : temp->properties){
                cout << "Key: \"" <<i.first << "\" Value: ";
                print({i.second}, env);
            }
            cout << "}";
        } 
        else if(args[i]->getType() == ValueType::Number){
            NumberVal* temp = (NumberVal*)args[i];
            cout << temp->val << ' ';
        }
        else if(args[i]->getType() == ValueType::Bool){
            BoolValue* temp = (BoolValue*)args[i];
            if(temp->val) cout << "True ";
            else cout << "False ";
        }
        else if(args[i]->getType() == ValueType::Null){
            NullVal* temp = (NullVal*)args[i];
            cout << temp->val << ' ';
        }
    }
    cout << '\n';
    return Make_Null();
}

Value *n_funs::timeNow(vector<Value*> args, Env *env){
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_c);

    // Extract hour and minute
    int hour = local_time->tm_hour;
    int minute = local_time->tm_min;
    return Make_Number(hour * 60 + minute);
}

Value* n_funs::floor(vector<Value*> args, Env *env){
    if(args.size() > 1 || args[0]->getType() != ValueType::Number){
        cout << "Floor Function: Wrong args";
        exit(0); // !!! debug systemi ile deyis
    }
    NumberVal* temp = (NumberVal*)args[0];
    long long res = temp->val;
    return Make_Number(res);
}
