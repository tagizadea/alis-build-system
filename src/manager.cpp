#include <manager.hpp>
#include <xxhash64.hpp>
#include <debug.hpp>
#include <algorithm>
#include <cstring>
#include <string_view>
#include <unordered_set>
using namespace std;

void Manager::init_manager(){

    auto& manager = Manager::getInstance();

    if(!fs::is_directory(".abs")) fs::create_directory(".abs");
    if(!fs::is_directory(".abs/plugins")) fs::create_directory(".abs/plugins");

    readCache<DependencyCacheEntry>(DEPS_CACHE_FILE_NAME, CacheType::DependencyCache, DependencyCache);
    readCache<FileCacheEntry>(FILES_CACHE_FILE_NAME, CacheType::FileCache, FileCache);
    readCache<ObjectCacheEntry>(OBJS_CACHE_FILE_NAME, CacheType::ObjectCache, ObjectCache);

    // int nese_count = 0;
    // for(auto& i : DependencyCache){
    //     cout << ++nese_count << ":\n" << i.first << '\n' << i.second.dependency_hash << '\n';
    //     for(auto& s : i.second.includes) cout << s << '\n';
    // }
    
    fstream about(".abs/about.txt", ios::out);
    if(!about.is_open()){
        ABS_FATAL(cat::Manager, "manager.about_open_fail");
    }
    about << "Ali's Build System for C/C++.\nVersion number: 1.1.7-beta";
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

// ============================================================
// Header scanning
// ------------------------------------------------------------
// The scanner is conservative on purpose: it records every #include it
// sees and does not evaluate #ifdef / #ifndef / #if conditions. A missed
// dependency gives a stale build, an extra one only costs a rebuild.
// The only branches skipped are ones that are dead for certain:
// #if 0, and the branches after an #if 1 / #elif 1.
//
// Includes are cached unresolved ('"name', '<name' or '#MACRO') and are
// resolved on every run, so include path changes and newly created
// headers are picked up without rescanning unchanged files.
// ============================================================

namespace{

inline bool isIdentStart(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

inline bool isIdentChar(char c){
    return isIdentStart(c) || (c >= '0' && c <= '9');
}

inline bool isHSpace(char c){
    return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

string normalizePath(const string& path){
    return fs::path(path).lexically_normal().generic_string();
}

// Translation phase 2: joins lines that end with a backslash.
void spliceLines(string& buf){
    if(buf.find('\\') == string::npos) return;
    size_t w = 0;
    for(size_t r = 0; r < buf.size(); ++r){
        if(buf[r] == '\\'){
            size_t n = r + 1;
            while(n < buf.size() && isHSpace(buf[n])) ++n;
            if(n < buf.size() && buf[n] == '\n'){
                r = n;
                continue;
            }
        }
        buf[w++] = buf[r];
    }
    buf.resize(w);
}

// Returns a pointer just past the "*/" that closes a block comment.
const char* skipBlockComment(const char* p, const char* end){
    while(p < end){
        const char* star = static_cast<const char*>(memchr(p, '*', end - p));
        if(!star || star + 1 >= end) return end;
        if(star[1] == '/') return star + 2;
        p = star + 1;
    }
    return end;
}

// Skips the rest of a "..." or '...' literal. Stops at a newline, so a stray
// apostrophe (e.g. prose inside #if 0) can't swallow the rest of the file.
const char* skipQuoted(const char* p, const char* end, char quote){
    while(p < end){
        if(*p == '\\' && p + 1 < end){
            p += 2;
            continue;
        }
        if(*p == quote) return p + 1;
        if(*p == '\n') return p;
        ++p;
    }
    return end;
}

// p points at the opening quote of R"delim( ... )delim".
// Returns p unchanged when it isn't a well-formed raw string.
const char* skipRawString(const char* p, const char* end){
    const char* delim = p + 1;
    const char* paren = delim;
    while(paren < end && *paren != '(' && paren - delim < 16){
        char c = *paren;
        if(c == ' ' || c == '\t' || c == '\n' || c == ')' || c == '\\' || c == '"') return p;
        ++paren;
    }
    if(paren >= end || *paren != '(') return p;

    string closing = ")";
    closing.append(delim, paren);
    closing += '"';
    string_view rest(paren + 1, end - paren - 1);
    size_t pos = rest.find(closing);
    return (pos == string_view::npos) ? end : paren + 1 + pos + closing.size();
}

// Copies one directive line (without the '#') into out, comments replaced
// by a space. Returns a pointer to the terminating newline (or end).
const char* readDirective(const char* p, const char* end, string& out){
    out.clear();
    while(p < end && *p != '\n'){
        if(*p == '/' && p + 1 < end && p[1] == '/'){
            const char* nl = static_cast<const char*>(memchr(p, '\n', end - p));
            return nl ? nl : end;
        }
        if(*p == '/' && p + 1 < end && p[1] == '*'){
            p = skipBlockComment(p + 2, end);
            out += ' ';
            continue;
        }
        if(*p == '"' || *p == '\''){
            const char* q = skipQuoted(p + 1, end, *p);
            out.append(p, q);
            p = q;
            continue;
        }
        out += *p++;
    }
    return p;
}

void skipSpaces(string_view s, size_t& i){
    while(i < s.size() && isHSpace(s[i])) ++i;
}

string_view readIdent(string_view s, size_t& i){
    size_t begin = i;
    while(i < s.size() && isIdentChar(s[i])) ++i;
    return s.substr(begin, i - begin);
}

enum class Literal{ False, True, Unknown };

// Only a plain 0 or 1 (optionally in parentheses) counts as a known condition.
Literal literalCondition(string_view expr){
    size_t b = 0, e = expr.size();
    while(true){
        while(b < e && isHSpace(expr[b])) ++b;
        while(e > b && isHSpace(expr[e - 1])) --e;
        if(e - b >= 2 && expr[b] == '(' && expr[e - 1] == ')'){
            ++b;
            --e;
            continue;
        }
        break;
    }
    if(e - b == 1 && expr[b] == '0') return Literal::False;
    if(e - b == 1 && expr[b] == '1') return Literal::True;
    return Literal::Unknown;
}

struct CondFrame{
    bool parentSkipping;
    bool taken;     // an earlier branch of this group is certainly taken
    bool skipping;
};

struct ScanState{
    vector<CondFrame> conds;
    vector<string> includes;
    vector<string> macros;  // flat name/value pairs
    string directive;       // reused line buffer

    bool skipping() const { return !conds.empty() && conds.back().skipping; }
};

void handleDirective(ScanState& st, string_view d){
    size_t i = 0;
    skipSpaces(d, i);
    string_view name = readIdent(d, i);
    if(name.empty()) return;

    if(name == "if" || name == "ifdef" || name == "ifndef"){
        bool parent = st.skipping();
        Literal lit = (name == "if") ? literalCondition(d.substr(i)) : Literal::Unknown;
        st.conds.push_back({parent, lit == Literal::True, parent || lit == Literal::False});
        return;
    }
    if(name == "elif" || name == "elifdef" || name == "elifndef"){
        if(st.conds.empty()) return;
        CondFrame& f = st.conds.back();
        Literal lit = (name == "elif") ? literalCondition(d.substr(i)) : Literal::Unknown;
        f.skipping = f.parentSkipping || f.taken || lit == Literal::False;
        if(lit == Literal::True) f.taken = true;
        return;
    }
    if(name == "else"){
        if(st.conds.empty()) return;
        CondFrame& f = st.conds.back();
        f.skipping = f.parentSkipping || f.taken;
        return;
    }
    if(name == "endif"){
        if(!st.conds.empty()) st.conds.pop_back();
        return;
    }

    if(st.skipping()) return;

    if(name == "include" || name == "include_next" || name == "import"){
        skipSpaces(d, i);
        if(i >= d.size()) return;
        char open = d[i];
        if(open == '"' || open == '<'){
            size_t close = d.find(open == '"' ? '"' : '>', i + 1);
            if(close == string_view::npos || close == i + 1) return;
            string spec(1, open);
            spec.append(d.substr(i + 1, close - i - 1));
            st.includes.push_back(move(spec));
        }
        else if(isIdentStart(open)){
            string_view macro = readIdent(d, i);
            skipSpaces(d, i);
            if(i < d.size() && d[i] == '(') return;  // function-like macro, can't expand
            st.includes.push_back("#" + string(macro));
        }
        return;
    }

    if(name == "define"){
        skipSpaces(d, i);
        string_view macro = readIdent(d, i);
        if(macro.empty() || (i < d.size() && d[i] == '(')) return;  // function-like
        skipSpaces(d, i);
        string_view value = d.substr(i);
        while(!value.empty() && isHSpace(value.back())) value.remove_suffix(1);

        // Only header-name shaped values can matter for "#include MACRO"
        if(value.size() < 3) return;
        bool quoted = value.front() == '"' && value.back() == '"';
        bool angled = value.front() == '<' && value.back() == '>';
        if(!quoted && !angled) return;
        if(value.find_first_of(" \t") != string_view::npos) return;
        st.macros.emplace_back(macro);
        st.macros.emplace_back(value);
    }
}

void scanText(const string& buf, ScanState& st){
    const char* p = buf.data();
    const char* end = p + buf.size();
    bool lineStart = true;

    while(p < end){
        char c = *p;
        if(c == '\n'){
            lineStart = true;
            ++p;
            continue;
        }
        if(isHSpace(c)){
            ++p;
            continue;
        }
        if(c == '/' && p + 1 < end && p[1] == '/'){
            const char* nl = static_cast<const char*>(memchr(p, '\n', end - p));
            p = nl ? nl : end;
            continue;
        }
        if(c == '/' && p + 1 < end && p[1] == '*'){
            p = skipBlockComment(p + 2, end);
            continue;
        }
        if(c == '#' && lineStart){
            p = readDirective(p + 1, end, st.directive);
            handleDirective(st, st.directive);
            continue;
        }
        lineStart = false;

        if(c == '"' || c == '\''){
            p = skipQuoted(p + 1, end, c);
            continue;
        }
        if(c >= '0' && c <= '9'){
            // pp-number, so the digit separator in 1'000 isn't read as a char literal
            ++p;
            while(p < end && (isIdentChar(*p) || *p == '.' || (*p == '\'' && p + 1 < end && isIdentChar(p[1])))) ++p;
            continue;
        }
        if(isIdentStart(c)){
            const char* begin = p;
            while(p < end && isIdentChar(*p)) ++p;
            if(p < end && *p == '"'){
                string_view prefix(begin, p - begin);
                if(prefix == "R" || prefix == "LR" || prefix == "uR" || prefix == "UR" || prefix == "u8R") p = skipRawString(p, end);
            }
            continue;
        }
        ++p;
    }
}

// Turns a cached include spec into a normalized path, or "" when the header
// isn't part of the project (system headers, unknown macros, missing files).
string resolveInclude(const string& spec, const string& dir, const vector<string>& includePaths,
                      const unordered_map<string, string>& macros, unordered_map<string, string>& memo){
    char kind = spec[0];
    string name = spec.substr(1);

    // #include MACRO: follow the macro to a "header" or <header>
    for(int depth = 0; kind == '#' && depth < 8; ++depth){
        auto macro_it = macros.find(name);
        if(macro_it == macros.end()) return "";
        const string& value = macro_it->second;
        if(value.size() >= 2 && ((value.front() == '"' && value.back() == '"') || (value.front() == '<' && value.back() == '>'))){
            kind = value.front();
            name = value.substr(1, value.size() - 2);
        }
        else if(macros.count(value)) name = value;
        else{
            kind = '"';
            name = value;
        }
    }
    if(kind == '#' || name.empty()) return "";

    string key(1, kind);
    if(kind == '"'){
        key += dir;
        key += '\0';
    }
    key += name;
    auto memo_it = memo.find(key);
    if(memo_it != memo.end()) return memo_it->second;

    string result;
    error_code ec;
    auto tryCandidate = [&](const fs::path& candidate){
        if(!fs::is_regular_file(candidate, ec)) return false;
        result = candidate.lexically_normal().generic_string();
        return true;
    };

    fs::path header(name);
    if(header.is_absolute()) tryCandidate(header);
    else{
        // Quoted includes look next to the including file first, like the compiler does
        bool found = (kind == '"') && tryCandidate(fs::path(dir) / header);
        for(size_t i = 0; !found && i < includePaths.size(); ++i) found = tryCandidate(fs::path(includePaths[i]) / header);
    }

    memo.emplace(move(key), result);
    return result;
}

} // namespace

bool Manager::scan_headers(const string& File, DependencyCacheEntry& Entry){
    ABS_PROFILE_FUNC();
    ifstream file(fs::path(File), ios::binary);
    if(!file.is_open()) return false;

    file.seekg(0, ios::end);
    streamoff size = file.tellg();
    if(size < 0) return false;
    string buf(static_cast<size_t>(size), '\0');
    file.seekg(0, ios::beg);
    if(size > 0 && !file.read(&buf[0], size)) return false;
    file.close();

    spliceLines(buf);

    ScanState st;
    scanText(buf, st);

    sort(st.includes.begin(), st.includes.end());
    st.includes.erase(unique(st.includes.begin(), st.includes.end()), st.includes.end());

    Entry.includes = move(st.includes);
    Entry.macros = move(st.macros);
    return true;
}

vector<string> Manager::scan_graph(const vector<string>& Sources){
    ABS_PROFILE_FUNC();
    auto& manager = Manager::getInstance();

    // Header-name macros for "#include MACRO". Script define()s win over file #defines.
    unordered_map<string, string> macros;
    for(const auto& [name, entry] : manager.DependencyCache){
        for(size_t i = 0; i + 1 < entry.macros.size(); i += 2) macros[entry.macros[i]] = entry.macros[i + 1];
    }
    for(const auto& [name, value] : manager.defines){
        if(value.has_value()) macros[name] = value.value();
    }
    auto scriptDefined = [&](const string& name){
        auto def_it = manager.defines.find(name);
        return def_it != manager.defines.end() && def_it->second.has_value();
    };

    // Graph nodes are normalized paths. Sources keep the spelling the script
    // used, because compile() matches tracked_src against its src list as written.
    unordered_map<string, string> sourceNames;
    unordered_set<string> visited;
    vector<string> stack;
    for(const string& s : Sources){
        string node = normalizePath(s);
        sourceNames.emplace(node, s);
        if(visited.insert(node).second) stack.push_back(node);
    }

    unordered_map<string, vector<string>> reverse_graph;
    unordered_map<string, string> resolved;
    vector<string> changed;

    while(!stack.empty()){
        string node = move(stack.back());
        stack.pop_back();

        auto src_it = sourceNames.find(node);
        const string& path = (src_it != sourceNames.end()) ? src_it->second : node;

        if(manager.track(path).mtime != 0) changed.push_back(node);
        auto file_it = manager.FileCache.find(path);
        if(file_it == manager.FileCache.end()) continue;  // file doesn't exist

        // Rescan only when the content changed since the last scan
        auto dep_it = manager.DependencyCache.find(node);
        if(dep_it == manager.DependencyCache.end() || dep_it->second.dependency_hash != file_it->second.content_hash){
            DependencyCacheEntry entry;
            if(!scan_headers(path, entry)) continue;
            entry.name = node;
            entry.dependency_hash = file_it->second.content_hash;
            for(size_t i = 0; i + 1 < entry.macros.size(); i += 2){
                if(!scriptDefined(entry.macros[i])) macros[entry.macros[i]] = entry.macros[i + 1];
            }
            dep_it = manager.DependencyCache.insert_or_assign(node, move(entry)).first;
        }

        string dir = fs::path(node).parent_path().generic_string();
        for(const string& spec : dep_it->second.includes){
            string target = resolveInclude(spec, dir, manager.include_paths, macros, resolved);
            if(target.empty() || target == node) continue;
            reverse_graph[target].push_back(node);
            if(visited.insert(target).second) stack.push_back(target);
        }
    }

    set<string> affected = reverse_invalidation(reverse_graph, changed);

    vector<string> dirty;
    unordered_set<string> added;
    for(const string& s : Sources){
        string node = normalizePath(s);
        if(affected.count(node) && added.insert(node).second) dirty.push_back(s);
    }
    return dirty;
}

set<string> Manager::reverse_invalidation(const unordered_map<string, vector<string>>& reverse_graph, const vector<string>& ChangedFiles){
    ABS_PROFILE_FUNC();
    set<string> color_reverse(ChangedFiles.begin(), ChangedFiles.end());
    vector<string> stack_reverse(ChangedFiles.begin(), ChangedFiles.end());

    while(!stack_reverse.empty()){
        string node = move(stack_reverse.back());
        stack_reverse.pop_back();

        auto rg_it = reverse_graph.find(node);
        if(rg_it == reverse_graph.end()) continue;
        for(const string& s : rg_it->second){
            if(color_reverse.insert(s).second) stack_reverse.push_back(s);
        }
    }

    return color_reverse;
}
