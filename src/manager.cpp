#include <manager.hpp>

template<typename Entry, typename Cache>
void readCache(const string& filename, Cache& cache){
    const string path = ".abs/" + filename;

    if(!fs::exists(path)){
        {
            fstream create(path, ios::out | ios::binary);
            size_t size = 0;
            writeBinary(create, size);
        }
    }

    fstream temp_fstr(path, ios::in | ios::binary);

    if(!temp_fstr.is_open()){
        cout << "Manager error: The file \"" << filename << "\" could not be opened!";
        exit(0);
    }

    size_t size = 0;
    readBinary(temp_fstr, size);

    for(size_t i = 0; i < size; ++i){
        Entry temp;
        temp.load(temp_fstr);
        cache.insert({temp.name, temp});
    }
}


void Manager::init_manager(){
    // generate_crc32_table();

    // size_t size;

    if(!fs::is_directory(".abs")) fs::create_directory(".abs");

    // fstream tracked;
    // if(!fs::exists(".abs/tracked.txt")) tracked.open(".abs/tracked.txt", ios::in | ios::out | ios::app);
    // else tracked.open(".abs/tracked.txt", ios::in | ios::out);
    // if(!tracked.is_open()){
    //     cout << "Manager error: The file \"tracked\" could not opened!";
    //     exit(0); // !!! Debug systemi ile deyis
    // }


    readCache<DependencyCacheEntry>("dependencies.cache", DependencyCache);
    readCache<FileCacheEntry>("files.cache", FileCache);
    readCache<ObjectCacheEntry>("objects.cache", ObjectCache);
    
    // fstream deps;
    // if(!fs::exists(".abs/dependencies")) deps.open(".abs/dependencies", ios::binary);
    // else deps.open(".abs/dependencies", ios::binary);
    // if(!deps.is_open()){
    //     cout << "Manager error: The file \"dependencies\" could not opened!";
    //     exit(0); // !!! Debug systemi ile deyis
    // }

    // readBinary(deps, size);

    // for(int i = 0; i < size; ++i){
    //     DependencyCacheEntry temp;
    //     temp.load(deps);
    //     DependencyCache.insert(temp.name, temp);
    // }

    // deps.close();

    fstream about(".abs/about.txt", ios::out);
    if(!about.is_open()){
        cout << "Manager error: The file \"about\" could not opened!";
        exit(0); // !!! Debug systemi ile deyis
    }
    about << "Ali's Build System for C/C++.\nVersion number: 0.8ALPHA";
    about.close();

    // string line;
    // int line_cnt = 0;
    // while(getline(tracked, line)){
    //     if(line.size() <= 2) continue;
    //     string name = "", checksum = "";
    //     bool flag = false;
    //     for(int i=0;i<line.size();++i){

    //         if(line[i] == ' '){
    //             flag = true;
    //             continue;
    //         }

    //         if(flag) checksum += line[i];
    //         else name += line[i];
    //     }
    //     try{
    //         Manager::getInstance().tracked_files[name] = {++line_cnt, static_cast<uint32_t>(stoul(checksum))};
    //     }
    //     catch(const std::exception& e){
    //         cout << "Manager error: \"tracked\" file corrupted! " << e.what() << '\n';
    //         exit(0); // !!! Debug systemi ile deyis
    //     }
    // }
    // tracked.close();
    // for(auto i : Manager::getInstance().tracked_files) cout << i.first << ' ' << i.second.first << ' ' << i.second.second << '\n'; //
}

vector<string> track(){
    vector <string> ans;

    // for(string s : Manager::getInstance().sources){
    //     if(fs::exists(s)){
    //         ifstream file(s);
            // uint32_t checksum = compute_crc32(file);
    //         uint32_t saved_checksum = 0;
    //         bool first_time = false;
    //         if(Manager::getInstance().tracked_files.count(s))
    //             saved_checksum = Manager::getInstance().tracked_files[s].second;
    //         else first_time = true;
            
    //         if(checksum != saved_checksum){
    //             ans.push_back(s);
    //             Manager::getInstance().tracked_files[s].second = checksum;
    //             if(first_time){
    //                 fstream tracked(".abs/tracked.txt", ios::app);
    //                 tracked << s << ' ' << checksum << '\n';
    //                 tracked.close();
    //             }
    //             else{
    //                 int line = Manager::getInstance().tracked_files[s].first;
    //                 fstream tracked(".abs/tracked.txt", ios::in | ios::out);
    //                 string l;
    //                 int c_l = 0;
    //                 vector <string> temp;
    //                 while(getline(tracked, l)){
    //                     ++c_l;
    //                     if(c_l == line) temp.push_back( s + " " + to_string(checksum));
    //                     else temp.push_back(l);
    //                 }
    //                 tracked.clear();
    //                 tracked.seekp(0, ios::beg);
    //                 for(string si : temp){
    //                     tracked << si << '\n';
    //                 }
    //                 tracked.close();
    //             }
    //         }

    //         file.close();
    //     }
    // }

    return ans;
}

// Nfunc kimi de olsun, compile cagirsin ve cagiranda trackeddaki cppleri de tapib gondersin
// fstream deps(".abs/dependencies.txt", ios::app);

string trimTrailing(string s){
    size_t last = s.find_last_not_of(" \t\r\n");
    if(last == string::npos) return "";
    return s.substr(0, last + 1);
}

bool isAllWhitespace(const string& s){
    return s.find_first_not_of(" \t\r\n") == string::npos;
}

pair<string, string> extractDefineParts(const string& line){
    size_t pos = line.find("#define");
    if (pos == string::npos || !isAllWhitespace(line.substr(0, pos))) return {"", ""};

    size_t nameStart = line.find_first_not_of(" \t", pos + 7);
    if (nameStart == string::npos) return {"", ""};

    size_t nameEnd = line.find_first_of(" \t(", nameStart);
    string name = (nameEnd == string::npos) ? line.substr(nameStart) : line.substr(nameStart, nameEnd - nameStart);

    string value = "";
    if(nameEnd != string::npos && line[nameEnd] != '('){
        size_t valStart = line.find_first_not_of(" \t", nameEnd);
        if(valStart != string::npos) {
            size_t commentPos = line.find("//", valStart);
            value = line.substr(valStart, commentPos - valStart);
            value = trimTrailing(value);
        }
    }
    return {name, value};
}

string extractDirectiveTarget(const string& line, const string& command){
    size_t pos = line.find(command);
    if(pos == string::npos || !isAllWhitespace(line.substr(0, pos))) return "";

    size_t nameStart = line.find_first_not_of(" \t", pos + command.length());
    if(nameStart == string::npos) return "";

    size_t nameEnd = line.find_first_of(" \t(/", nameStart);
    return (nameEnd == string::npos) ? line.substr(nameStart) : line.substr(nameStart, nameEnd - nameStart);
}

string extractHeaderName(const string& line){
    size_t includePos = line.find("#include");
    if(includePos == string::npos || !isAllWhitespace(line.substr(0, includePos))) return "";

    size_t startDelimPos = line.find_first_of("<\"", includePos + 8);
    if(startDelimPos != string::npos){
        char closingChar = (line[startDelimPos] == '<') ? '>' : '\"';
        size_t endDelimPos = line.find(closingChar, startDelimPos + 1);
        if(endDelimPos != string::npos){
            return line.substr(startDelimPos + 1, endDelimPos - startDelimPos - 1);
        }
    }

    string mName = extractDirectiveTarget(line, "#include");
    if(!mName.empty()){
        auto& defs = Manager::getInstance().defines;
        if(defs.count(mName) && defs[mName].has_value()){
            string res = defs[mName].value();
            if(res.size() >= 2 && (res.front() == '\"' || res.front() == '<')) return res.substr(1, res.size() - 2);
            return res;
        }
    }
    return "";
}

vector<string> scan_headers(){
    set<string> uniqueHeaders;
    auto& manager = Manager::getInstance();

    for(const string& filePath : manager.sources){
        fstream file(filePath, ios::in);
        if(!file.is_open()) continue;

        string rawLine, combinedLine;
        vector<bool> skipStack;
        bool inBlockComment = false;

        while(getline(file, rawLine)){
            string trimmedRaw = trimTrailing(rawLine);
            if(!trimmedRaw.empty() && trimmedRaw.back() == '\\'){
                combinedLine += trimmedRaw.substr(0, trimmedRaw.size() - 1);
                continue; 
            }
            else combinedLine += rawLine;

            string line = combinedLine;
            combinedLine = "";

            if(!inBlockComment){
                size_t startCmt = line.find("/*");
                if(startCmt != string::npos){
                    if(line.find("*/", startCmt + 2) == string::npos) inBlockComment = true;
                }
            }
            else{
                if(line.find("*/") != string::npos) inBlockComment = false;
                continue;
            }
            if(inBlockComment) continue;

            string ifndefMacro = extractDirectiveTarget(line, "#ifndef");
            if(!ifndefMacro.empty()){
                bool alreadyDef = manager.defines.count(ifndefMacro);
                bool parentSkip = (!skipStack.empty() && skipStack.back());
                skipStack.push_back(parentSkip || alreadyDef);
                continue;
            }

            string ifdefMacro = extractDirectiveTarget(line, "#ifdef");
            if(!ifdefMacro.empty()){
                bool notDef = !manager.defines.count(ifdefMacro);
                bool parentSkip = (!skipStack.empty() && skipStack.back());
                skipStack.push_back(parentSkip || notDef);
                continue;
            }

            size_t endifPos = line.find("#endif");
            if(endifPos != string::npos && isAllWhitespace(line.substr(0, endifPos))){
                if(!skipStack.empty()) skipStack.pop_back();
                continue;
            }

            if(!skipStack.empty() && skipStack.back()) continue;

            string undefName = extractDirectiveTarget(line, "#undef");
            if(!undefName.empty()){
                manager.defines.erase(undefName);
                continue;
            }

            auto [defName, defVal] = extractDefineParts(line);
            if(!defName.empty()){
                manager.defines[defName] = defVal.empty() ? nullopt : make_optional(defVal);
                continue;
            }

            string header = extractHeaderName(line);
            if(!header.empty()) uniqueHeaders.insert(header);
        }
    }
    
    return vector<string>(uniqueHeaders.begin(), uniqueHeaders.end());
}