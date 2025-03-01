#include <operations.hpp>
#include <chrono>
#include <ctime>
#include <queue>

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



Value* n_funs::print(vector <Value*> args, Env* env){ // naive print fun
    queue <pair <vector <Value*> , string> > q;
    q.push({args, ""});
    while(!q.empty()){
        vector <Value*> v = q.front().first;
        string tab_s = q.front().second;

        for(int i=0;i<v.size();++i){
            if(v[i]->getType() == ValueType::Object){
                ObjectValue* temp = (ObjectValue*)v[i];
                cout << tab_s << "Object\n";
    
                for(pair <string, Value*> i : temp->properties){
                    cout << tab_s + "    "<< "Key: \"" <<i.first << "\" Value: ";
                    //print({i.second}, env);
                    q.push({{i.second}, tab_s + "    "});
                }
                //cout << tab_s << "}\n";
                cout << '\n';
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
    return Make_Null();
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
        cout << "System Function: Wrong args. Use platform specific commans";
        exit(0); // !!! debug systemi ile deyis
    }
    
    StringVal* temp = (StringVal*)args[0];
    std::system(temp->val.c_str());
    return Make_Null();
}
