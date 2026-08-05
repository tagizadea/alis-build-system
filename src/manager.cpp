#include <manager.hpp>
using namespace std;
#include <xxhash64.hpp>
#include <debug.hpp>
#include <stack>

void Manager::init_manager(){

    auto& manager = Manager::getInstance();

    if(!fs::is_directory(".abs")) fs::create_directory(".abs");
    if(!fs::is_directory(".abs/plugins")) fs::create_directory(".abs/plugins");

    readCache<DependencyCacheEntry>(DEPS_CACHE_FILE_NAME, DependencyCache);
    readCache<FileCacheEntry>(FILES_CACHE_FILE_NAME, FileCache);
    readCache<ObjectCacheEntry>(OBJS_CACHE_FILE_NAME, ObjectCache);

    // int nese_count = 0;
    // for(auto& i : DependencyCache){
    //     cout << ++nese_count << ":\n" << i.first << '\n' << i.second.dependency_hash << '\n';
    //     for(auto& s : i.second.includes) cout << s << '\n';
    // }
    
    fstream about(".abs/about.txt", ios::out);
    if(!about.is_open()){
        ABS_FATAL(cat::Manager, "manager.about_open_fail");
    }
    about << "Ali's Build System for C/C++.\nVersion number: 1.0.0-beta";
    about.close();

    manager.include_paths.push_back("./");
}

Manager::FileCacheEntry Manager::track(const string& Source){
    ABS_PROFILE_FUNC();
    Manager::FileCacheEntry ans;
    ans.content_hash = 0;
    ans.mtime = 0;
    ans.name = "";
    ans.size = 0;

    auto& FileCache = Manager::getInstance().FileCache;

    // for(string s : Manager::getInstance().sources){

    // cout << "!!!FOR: " + s + "!!!\n"; //

    if(fs::exists(Source)){

        // cout << "!!!EXISTS: " + s + "!!!\n"; //
        // ifstream file(s);

        uint64_t mtime = GetFileMTime(Source), size = GetFileSize(Source);

        if(FileCache.count(Source)){
            auto entry = FileCache[Source];

            // cout << "!!!FILE CACHE TAPILDI: " + s + "!!!\n"; //

            // cout << entry.name << ' ' << entry.content_hash << ' ' << entry.mtime <<
            // ' ' << entry.size << '\n'; //

            if((mtime != entry.mtime || size != entry.size)){
                uint64_t contentHash = xxh64::hash_file(Source);
                entry.mtime = mtime;
                entry.size = size;
                if(contentHash != entry.content_hash){
                    entry.content_hash = contentHash;
                    // ans.push_back(Source);
                    ans = entry;
                }
                FileCache[Source] = entry;
            }
        }
        else{
            auto entry = FileCache[Source];

            // cout << "!!! ILK DEFE " + s + "!!!\n"; //

            entry.content_hash = xxh64::hash_file(Source);
            entry.mtime = mtime;
            entry.name = Source;
            entry.size = size;
            // ans.push_back(Source);
            ans = entry;
            FileCache[Source] = entry;
        }
        // file.close();
    }
    // }

    // writeCache<FileCacheEntry>(FILES_CACHE_FILE_NAME, FileCache);
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

vector<string> Manager::scan_headers(const string& ChangedFile){
    ABS_PROFILE_FUNC();
    set<string> uniqueHeaders;
    string dephash = "";
    auto& manager = Manager::getInstance();

    fstream file;
    string temp_path;

    for(const string& s : manager.include_paths){
        temp_path = s + ((s[s.size() - 1] == '/') ? ("") : ("/")) + ChangedFile;
        if(fs::exists(temp_path)){
            file.open(temp_path, ios::in);
            break;
        }
    }

    if(!file.is_open()){
        // cout << "Manager: " + ChangedFile + " cannot be opened!\n";
        // exit(0); // !!! debug systemi ile deyis
        return vector <string>();
    }

    string rawLine, combinedLine;
    vector<bool> skipStack;
    bool inBlockComment = false;

    Manager::DependencyCacheEntry entry;
    entry.name = temp_path;

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
        if(!header.empty()){
            string resolved = header;
            for(const string& path : manager.include_paths){
                string candidate = path + (path.back() == '/' ? "" : "/") + header;
                if(fs::exists(candidate)){
                    resolved = candidate;
                    break;
                }
            }
            uniqueHeaders.insert(header);
            // Hash the RESOLVED path, not the raw header name, so two files
            // including different headers with the same filename produce
            // different dependency hashes.
            dephash += resolved;
            entry.includes.push_back(resolved);
        }

    }
    
    
    entry.dependency_hash = xxh64::hash_string(dephash);

    if(manager.DependencyCache.count(temp_path) && manager.DependencyCache[temp_path].dependency_hash == entry.dependency_hash) return vector<string>();
    
    manager.DependencyCache[temp_path] = entry;
    // writeCache<DependencyCacheEntry>(DEPS_CACHE_FILE_NAME, manager.DependencyCache);

    for(const string& s : uniqueHeaders){
        auto temp = scan_headers(s);
        for(const string& i : temp) uniqueHeaders.insert(i);
    }

    return vector<string>(uniqueHeaders.begin(), uniqueHeaders.end());
}

bool IsSourceFile(const string& name){
    string ext = fs::path(name).extension().string();
    // Normalize to lowercase for case-insensitive comparison.
    for(char& c : ext) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return ext == ".c" || ext == ".cc" || ext == ".cpp" || ext == ".cxx" || ext == ".c++";
}

vector <string> Manager::scan_graph(const DependencyCacheEntry& Node, set <string>& color, map <string, vector <string>>& reverse_graph){
    auto& manager = Manager::getInstance();
    vector <string> DirtyHeaderNames;
    // map <string, vector <string>> reverse_graph;

    stack <DependencyCacheEntry> Stack;

    Stack.push(Node);
    color.insert(Node.name);

    while(!Stack.empty()){
        const DependencyCacheEntry& temp_Node = Stack.top();

        for(const string& i : temp_Node.includes){

            reverse_graph[i].push_back(temp_Node.name);
            
            if(color.find(i) == color.end()){
                // Use find() instead of operator[] to avoid default-constructing
                // a DependencyCacheEntry with dependency_hash=0 when the key
                // doesn't exist yet (which would corrupt the cache).
                auto dep_it = manager.DependencyCache.find(i);
                if(dep_it != manager.DependencyCache.end()){
                    Stack.push(dep_it->second);
                }
                color.insert(i);
            
                if(!IsSourceFile(i)){
                    FileCacheEntry temp_Entry = track(i);
            
                    if(temp_Entry.mtime != 0){
                        manager.FileCache[i] = temp_Entry;
                        DirtyHeaderNames.push_back(i);
                    }

                }
            }
        }

        Stack.pop();
    }

    return DirtyHeaderNames;
}

vector<string> Manager::reverse_invalidation(map<string, vector<string>>& reverse_graph, vector <string>& DirtyHeaderNames){
    ABS_PROFILE_FUNC();
    set <string> color_reverse;

    for(const string& i : DirtyHeaderNames){

        if(color_reverse.find(i) == color_reverse.end()){
            stack <string> stack_reverse;
            stack_reverse.push(i);
            color_reverse.insert(i);

            while(!stack_reverse.empty()){
                const string& temp_Reverse_Node = stack_reverse.top();
                

                // Use find() instead of operator[] to avoid default-constructing
                // an empty vector for keys that don't exist in the reverse graph.
                auto rg_it = reverse_graph.find(temp_Reverse_Node);
                if(rg_it != reverse_graph.end()){
                    for(const string& s : rg_it->second){
                        if(color_reverse.find(s) == color_reverse.end()){
                            color_reverse.insert(s);
                            stack_reverse.push(s);
                        }
                    }
                }

                stack_reverse.pop();
            }

        }
    }

    vector <string> ShouldCompile;
    for(const string& i : color_reverse) if(IsSourceFile(i)) ShouldCompile.push_back(i);
    return ShouldCompile;
}
