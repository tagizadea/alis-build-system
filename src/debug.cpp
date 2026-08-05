#include <debug.hpp>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <mutex>

namespace debug {

// ---------------------------------------------------------------------------
//  Internal state
// ---------------------------------------------------------------------------

struct Debugger::Impl {
    Lang language = Lang::EN;
    Level globalLevel = Level::Trace;
    std::unordered_map<Category, Level> categoryLevels;
    std::ostream* out = &std::cerr;
    bool useColor = true;

    // Script context stack (for nested run() calls).
    std::vector<std::string> scriptStack;
    int scriptLine = 0;
    int scriptCol = 0;

    // Message catalog: id -> { EN, AZ }
    std::unordered_map<std::string, std::pair<std::string, std::string>> catalog;

    // Profiling stats
    bool profilingEnabled = false;
    struct ProfileEntry {
        long long count = 0;
        std::chrono::steady_clock::duration total{0};
        std::chrono::steady_clock::duration min{std::chrono::steady_clock::duration::max()};
        std::chrono::steady_clock::duration max{0};
    };
    std::unordered_map<std::string, ProfileEntry> profile;

    // Memory tracking
    bool memTrackingEnabled = false;
    std::size_t allocBytes = 0;
    std::size_t allocCount = 0;
    std::size_t freeBytes = 0;
    std::size_t freeCount = 0;
    std::size_t peakBytes = 0;

    std::mutex mtx;
};

// ---------------------------------------------------------------------------
//  Singleton
// ---------------------------------------------------------------------------

Debugger& Debugger::getInstance(){
    static Debugger instance;
    return instance;
}

// ---------------------------------------------------------------------------
//  Language
// ---------------------------------------------------------------------------

Lang Debugger::langFromString(const std::string& s){
    std::string up = s;
    std::transform(up.begin(), up.end(), up.begin(),
        [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
    if(up == "AZ" || up == "AZERBAIJANI" || up == "AZERBAYCAN") return Lang::AZ;
    return Lang::EN;
}

Lang Debugger::getLanguage() const { return impl_->language; }

void Debugger::setLanguage(Lang lang){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->language = lang;
}

// ---------------------------------------------------------------------------
//  Severity control
// ---------------------------------------------------------------------------

void Debugger::setLevel(Level lvl){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->globalLevel = lvl;
}

void Debugger::setCategoryLevel(Category cat, Level lvl){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->categoryLevels[cat] = lvl;
}

bool Debugger::isEnabled(Level lvl, Category cat) const{
    if(static_cast<int>(lvl) < static_cast<int>(impl_->globalLevel)) return false;
    auto it = impl_->categoryLevels.find(cat);
    if(it != impl_->categoryLevels.end()){
        if(static_cast<int>(lvl) < static_cast<int>(it->second)) return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
//  Output control
// ---------------------------------------------------------------------------

void Debugger::setOutputStream(std::ostream& os){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->out = &os;
}

void Debugger::setUseColor(bool enabled){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->useColor = enabled;
}

// ---------------------------------------------------------------------------
//  Script context
// ---------------------------------------------------------------------------

void Debugger::setScriptContext(const std::string& path){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->scriptStack.push_back(path);
    impl_->scriptLine = 0;
    impl_->scriptCol = 0;
}

void Debugger::popScriptContext(){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    if(!impl_->scriptStack.empty()) impl_->scriptStack.pop_back();
    impl_->scriptLine = 0;
    impl_->scriptCol = 0;
}

void Debugger::setScriptLine(int line){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->scriptLine = line;
}

void Debugger::setScriptCol(int col){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->scriptCol = col;
}

std::string Debugger::getScriptPath() const{
    if(impl_->scriptStack.empty()) return "";
    return impl_->scriptStack.back();
}

// ---------------------------------------------------------------------------
//  Message catalog
// ---------------------------------------------------------------------------

void Debugger::registerMessage(const std::string& id, Lang lang, std::string fmt){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    auto& entry = impl_->catalog[id];
    if(lang == Lang::AZ) entry.second = std::move(fmt);
    else entry.first = std::move(fmt);
}

void registerMessage(const std::string& id, Lang lang, std::string fmt){
    Debugger::getInstance().registerMessage(id, lang, std::move(fmt));
}

std::string Debugger::getTemplate(const std::string& msgId) const{
    auto it = impl_->catalog.find(msgId);
    if(it == impl_->catalog.end()){
        // Return the raw id so the caller still sees something useful.
        return "[" + msgId + "] (no message template registered)";
    }
    if(impl_->language == Lang::AZ && !it->second.second.empty()) return it->second.second;
    return it->second.first.empty() ? it->second.first : (it->second.second.empty() ? msgId : it->second.first);
}

// ---------------------------------------------------------------------------
//  Core log
// ---------------------------------------------------------------------------

static const char* levelStr(Level lvl){
    switch(lvl){
        case Level::Trace:   return "TRACE";
        case Level::Debug:   return "DEBUG";
        case Level::Info:    return "INFO";
        case Level::Warning: return "WARNING";
        case Level::Error:   return "ERROR";
        case Level::Fatal:   return "FATAL";
    }
    return "?";
}

static const char* categoryStr(Category cat){
    switch(cat){
        case Category::General:    return "GENERAL";
        case Category::Lexer:      return "LEXER";
        case Category::Parser:     return "PARSER";
        case Category::Eval:       return "EVAL";
        case Category::Env:        return "ENV";
        case Category::Manager:    return "MANAGER";
        case Category::Operations: return "OPS";
        case Category::Memory:     return "MEMORY";
        case Category::Performance:return "PERF";
        case Category::DSL:        return "DSL";
    }
    return "?";
}

static const char* colorFor(Level lvl){
    switch(lvl){
        case Level::Trace:   return "\033[90m"; // gray
        case Level::Debug:   return "\033[36m"; // cyan
        case Level::Info:    return "\033[32m"; // green
        case Level::Warning: return "\033[33m"; // yellow
        case Level::Error:   return "\033[31m"; // red
        case Level::Fatal:   return "\033[1;31m"; // bold red
    }
    return "\033[0m";
}

void Debugger::log(Level lvl, Category cat, const char* file, int line,
                   const char* func, const std::string& msgId, const std::string& msg){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    std::ostream& os = *impl_->out;
    if(impl_->useColor){
        os << colorFor(lvl);
    }
    os << '[' << levelStr(lvl) << "][" << categoryStr(cat) << ']';
    if(file && *file){
        os << ' ' << file << ':' << line;
        if(func && *func) os << " (" << func << ')';
    }
    // Script context: show the .abs file path and line/col if set.
    if(!impl_->scriptStack.empty()){
        os << " | " << impl_->scriptStack.back();
        if(impl_->scriptLine > 0){
            os << ':' << impl_->scriptLine;
            if(impl_->scriptCol > 0) os << ':' << impl_->scriptCol;
        }
    }
    os << " | " << msg << " [" << msgId << "]\n";
    if(impl_->useColor) os << "\033[0m";
    os.flush();
}

// ---------------------------------------------------------------------------
//  Profiling
// ---------------------------------------------------------------------------

void Debugger::profileStart(const char* name){
    impl_->profilingEnabled = true;
    (void)name;
}

void Debugger::profileEnd(const char* name, std::chrono::steady_clock::duration dur){
    auto& e = impl_->profile[name];
    ++e.count;
    e.total += dur;
    if(dur < e.min) e.min = dur;
    if(dur > e.max) e.max = dur;
}

void Debugger::dumpProfile(){
    if(impl_->profile.empty()) return;
    std::ostream& os = *impl_->out;
    os << "\n===== PROFILE SUMMARY =====\n";
    os << "name                          count    total(ms)   avg(ms)    min(ms)    max(ms)\n";
    for(const auto& [name, e] : impl_->profile){
        auto ms = [](auto d){ return std::chrono::duration_cast<std::chrono::microseconds>(d).count() / 1000.0; };
        char buf[256];
        std::snprintf(buf, sizeof(buf), "%-30s %6lld %10.3f %9.3f %9.3f %9.3f\n",
                      name.c_str(), (long long)e.count,
                      ms(e.total), e.count ? ms(e.total) / e.count : 0.0,
                      ms(e.min), ms(e.max));
        os << buf;
    }
    os << "============================\n";
}

// ---------------------------------------------------------------------------
//  Memory tracking
// ---------------------------------------------------------------------------

void Debugger::memAlloc(std::size_t bytes){
    if(!impl_->memTrackingEnabled) return;
    impl_->allocBytes += bytes;
    ++impl_->allocCount;
    if(impl_->allocBytes - impl_->freeBytes > impl_->peakBytes)
        impl_->peakBytes = impl_->allocBytes - impl_->freeBytes;
}

void Debugger::memFree(std::size_t bytes){
    if(!impl_->memTrackingEnabled) return;
    impl_->freeBytes += bytes;
    ++impl_->freeCount;
}

void Debugger::dumpMemory(){
    if(!impl_->memTrackingEnabled) return;
    std::ostream& os = *impl_->out;
    os << "\n===== MEMORY SUMMARY =====\n";
    os << "allocated: " << impl_->allocBytes << " bytes in " << impl_->allocCount << " allocations\n";
    os << "freed:     " << impl_->freeBytes << " bytes in " << impl_->freeCount << " frees\n";
    os << "peak live: " << impl_->peakBytes << " bytes\n";
    long long live = static_cast<long long>(impl_->allocBytes) - static_cast<long long>(impl_->freeBytes);
    os << "live (leaked if >0): " << live << " bytes\n";
    os << "==========================\n";
}

// ---------------------------------------------------------------------------
//  Scope timer
// ---------------------------------------------------------------------------

ScopeTimer::ScopeTimer(const char* name) : name_(name){
    start_ = std::chrono::steady_clock::now();
}

ScopeTimer::~ScopeTimer(){
    auto end = std::chrono::steady_clock::now();
    Debugger::getInstance().profileEnd(name_, end - start_);
}

// ---------------------------------------------------------------------------
//  Constructor & message catalog registration
// ---------------------------------------------------------------------------

Debugger::Debugger() : impl_(new Impl){
    // Default language from compile-time LANG macro.
    impl_->language = langFromString(LANG);

    // ---- Lexer ----

    // ---- Parser ----
    registerMessage("parser.eof_eat",            Lang::EN, "Reached end of tokens! Cannot eat further!");
    registerMessage("parser.eof_eat",            Lang::AZ, "Tokenlərin sonuna çatdın! Artıq eat etmək olmaz!");
    registerMessage("parser.expect_error",       Lang::EN, "Expected {0} but got {1} (token type {2})");
    registerMessage("parser.expect_error",       Lang::AZ, "Gözlənilən {0} amma alınan {1} (token növü {2})");
    registerMessage("parser.const_no_value",     Lang::EN, "Constant declaration requires a value!");
    registerMessage("parser.const_no_value",     Lang::AZ, "Konstant dəyişən dəyər olmadan təyin oluna bilməz!");
    registerMessage("parser.duplicate_var",      Lang::EN, "Cannot declare multiple variables with the same name!");
    registerMessage("parser.duplicate_var",      Lang::AZ, "Eyni adlı bir neçə dəyişən təyin oluna bilməz!");
    registerMessage("parser.missing_semicolon",  Lang::EN, "Variable declaration missing semicolon");
    registerMessage("parser.missing_semicolon",  Lang::AZ, "Dəyişən təyini semicolonsuz qalıb");
    registerMessage("parser.param_not_identifier", Lang::EN, "Function parameters must be identifiers");
    registerMessage("parser.param_not_identifier", Lang::AZ, "Funksiya parametrləri identifier tipində olmalıdır");
    registerMessage("parser.key_same_as_object", Lang::EN, "Object key cannot have the same name as its parent object");
    registerMessage("parser.key_same_as_object", Lang::AZ, "Açar adı valideyn obyektlə eyni ola bilməz");
    registerMessage("parser.unary_needs_identifier", Lang::EN, "Unary expression requires an identifier");
    registerMessage("parser.unary_needs_identifier", Lang::AZ, "Unar ifadə identifier tələb edir");
    registerMessage("parser.dot_needs_identifier", Lang::EN, "Dot operator requires an identifier");
    registerMessage("parser.dot_needs_identifier", Lang::AZ, "Nöqtə operatoru identifier olmadan işlənə bilməz");
    registerMessage("parser.minus_without_number", Lang::EN, "Minus operator used without a number");
    registerMessage("parser.minus_without_number", Lang::AZ, "Minus operatoru nömrəsiz işlədilib");
    registerMessage("parser.plus_without_number",  Lang::EN, "Plus operator used without a number");
    registerMessage("parser.plus_without_number",  Lang::AZ, "Plus operatoru nömrəsiz işlədilib");
    registerMessage("parser.unknown_expr",        Lang::EN, "Unknown expression encountered: {0}");
    registerMessage("parser.unknown_expr",        Lang::AZ, "Bilinməyən ifadə ilə qarşılaşıldı: {0}");

    // ---- Eval ----
    registerMessage("eval.mod_not_integer",      Lang::EN, "Modulo operator requires integer operands");
    registerMessage("eval.mod_not_integer",      Lang::AZ, "Kəsr ədədlərin MOD-u tapılmır!");
    registerMessage("eval.missing_args",         Lang::EN, "Missing arguments for function \"{0}\"");
    registerMessage("eval.missing_args",         Lang::AZ, "\"{0}\" funksiyası üçün arqumentlər çatışmır");
    registerMessage("eval.call_non_function",    Lang::EN, "Cannot call a value that is not a function");
    registerMessage("eval.call_non_function",    Lang::AZ, "Funksiya olmayan dəyər çağırıla bilməz");
    registerMessage("eval.assign_break_continue", Lang::EN, "Break or Continue cannot be assigned as a value");
    registerMessage("eval.assign_break_continue", Lang::AZ, "Break və ya Continue dəyər kimi təyin oluna bilməz");
    registerMessage("eval.assign_member_type",   Lang::EN, "Assignment with member access requires an Object or List value");
    registerMessage("eval.assign_member_type",   Lang::AZ, "Üzvə təyinat yalnız Object və List dəyərləri ilə işləyir");
    registerMessage("eval.assign_left_not_identifier", Lang::EN, "Left side of assignment is not an identifier");
    registerMessage("eval.assign_left_not_identifier", Lang::AZ, "Təyinatın sol tərəfi identifier deyil");
    registerMessage("eval.assign_right_break_continue", Lang::EN, "Right side of assignment cannot be break or continue");
    registerMessage("eval.assign_right_break_continue", Lang::AZ, "Təyinatın sağ tərəfində break və ya continue ola bilməz");
    registerMessage("eval.if_not_bool",          Lang::EN, "IF condition must be a boolean value");
    registerMessage("eval.if_not_bool",          Lang::AZ, "IF şərti mütləq boolean dəyər olmalıdır");
    registerMessage("eval.while_not_bool",       Lang::EN, "WHILE condition must be a boolean value");
    registerMessage("eval.while_not_bool",       Lang::AZ, "WHILE şərti mütləq boolean dəyər olmalıdır");
    registerMessage("eval.for_not_bool",         Lang::EN, "FOR condition must be a boolean value");
    registerMessage("eval.for_not_bool",         Lang::AZ, "FOR şərti mütləq boolean dəyər olmalıdır");
    registerMessage("eval.index_not_numeric",    Lang::EN, "Index must be numeric (got type {0})");
    registerMessage("eval.index_not_numeric",    Lang::AZ, "İndeks mütləq ədəd olmalıdır (alınan növ {0})");
    registerMessage("eval.index_out_of_bounds",  Lang::EN, "List index out of bounds: {0}");
    registerMessage("eval.index_out_of_bounds",  Lang::AZ, "List indeksi hüduddan kənardadır: {0}");
    registerMessage("eval.unknown_list_func",    Lang::EN, "Unknown list function");
    registerMessage("eval.unknown_list_func",    Lang::AZ, "Bilinməyən list funksiyası");
    registerMessage("eval.call_in_object",       Lang::EN, "Function call cannot be used directly inside an object literal");
    registerMessage("eval.call_in_object",       Lang::AZ, "Obyekt daxilində funksiya çağırışı işlədilə bilməz");
    registerMessage("eval.not_not_bool",         Lang::EN, "NOT operation requires a boolean value");
    registerMessage("eval.not_not_bool",         Lang::AZ, "NOT əməliyyatı yalnız boolean dəyərlər ilə işləyir");
    registerMessage("eval.continue_outside_loop", Lang::EN, "Continue used outside of a loop");
    registerMessage("eval.continue_outside_loop", Lang::AZ, "Continue dövrdən kənarda işlədilib");
    registerMessage("eval.break_outside_loop",   Lang::EN, "Break used outside of a loop");
    registerMessage("eval.break_outside_loop",   Lang::AZ, "Break dövrdən kənarda işlədilib");
    registerMessage("eval.unknown_type",         Lang::EN, "Encountered an unknown AST node type");
    registerMessage("eval.unknown_type",         Lang::AZ, "Bilinməyən AST qovşaq növü ilə qarşılaşıldı");

    // ---- Operations ----
    registerMessage("ops.pop_empty",             Lang::EN, "Cannot pop from an empty list");
    registerMessage("ops.pop_empty",             Lang::AZ, "List boşdur: pop icra oluna bilmir!");
    registerMessage("ops.list_multi_type",       Lang::EN, "List contains more than one type");
    registerMessage("ops.list_multi_type",       Lang::AZ, "List birdən çox tipdən ibarətdir");
    registerMessage("ops.floor_wrong_args",      Lang::EN, "floor() expects a single numeric argument");
    registerMessage("ops.floor_wrong_args",      Lang::AZ, "floor() bir ədəd arqument gözləyir");
    registerMessage("ops.max_multi_type",        Lang::EN, "max() requires a single-type list");
    registerMessage("ops.max_multi_type",        Lang::AZ, "max() yalnız bir tipli list qəbul edir");
    registerMessage("ops.max_not_comparable",    Lang::EN, "max() cannot compare values of this type");
    registerMessage("ops.max_not_comparable",    Lang::AZ, "max() bu tipdəki dəyərləri müqayisə edə bilmir");
    registerMessage("ops.max_wrong_args",        Lang::EN, "max() expects a single list argument");
    registerMessage("ops.max_wrong_args",        Lang::AZ, "max() yalnız bir list arqumenti qəbul edir");
    registerMessage("ops.min_multi_type",        Lang::EN, "min() requires a single-type list");
    registerMessage("ops.min_multi_type",        Lang::AZ, "min() yalnız bir tipli list qəbul edir");
    registerMessage("ops.min_not_comparable",    Lang::EN, "min() cannot compare values of this type");
    registerMessage("ops.min_not_comparable",    Lang::AZ, "min() bu tipdəki dəyərləri müqayisə edə bilmir");
    registerMessage("ops.min_wrong_args",        Lang::EN, "min() expects a single list argument");
    registerMessage("ops.min_wrong_args",        Lang::AZ, "min() yalnız bir list arqumenti qəbul edir");
    registerMessage("ops.system_wrong_args",     Lang::EN, "system() expects a single string argument");
    registerMessage("ops.system_wrong_args",     Lang::AZ, "system() bir sətir arqumenti gözləyir");
    registerMessage("ops.track_not_string",      Lang::EN, "track() accepts only string arguments");
    registerMessage("ops.track_not_string",      Lang::AZ, "track() yalnız sətir arqumentləri qəbul edir");
    registerMessage("ops.define_wrong_args",     Lang::EN, "define() accepts one or two arguments");
    registerMessage("ops.define_wrong_args",     Lang::AZ, "define() bir və ya iki arqument qəbul edir");
    registerMessage("ops.define_not_string",     Lang::EN, "define() arguments must be strings");
    registerMessage("ops.define_not_string",     Lang::AZ, "define() arqumentləri sətir olmalıdır");
    registerMessage("ops.type_wrong_args",       Lang::EN, "type() expects a single argument");
    registerMessage("ops.type_wrong_args",       Lang::AZ, "type() yalnız bir arqument qəbul edir");
    registerMessage("ops.ston_empty",            Lang::EN, "Nothing to convert to a number");
    registerMessage("ops.ston_empty",            Lang::AZ, "Nömrəyə çevriləcək dəyər yoxdur");
    registerMessage("ops.ntos_empty",            Lang::EN, "Nothing to convert to a string");
    registerMessage("ops.ntos_empty",            Lang::AZ, "Sətirə çevriləcək dəyər yoxdur");
    registerMessage("ops.ston_too_big",          Lang::EN, "Value \"{0}\" is too large to convert to a number");
    registerMessage("ops.ston_too_big",          Lang::AZ, "\"{0}\" dəyəri nömrəyə çevirmək üçün çox böyükdür");
    registerMessage("ops.ston_convert_fail",     Lang::EN, "Value \"{0}\" cannot be converted to a number");
    registerMessage("ops.ston_convert_fail",     Lang::AZ, "\"{0}\" dəyəri nömrəyə çevrilə bilmir");
    registerMessage("ops.ston_not_string",       Lang::EN, "Value is not a string, cannot convert to number");
    registerMessage("ops.ston_not_string",       Lang::AZ, "Dəyər sətir deyil, nömrəyə çevrilə bilməz");
    registerMessage("ops.ntos_convert_fail",     Lang::EN, "Value {0} cannot be converted to a string");
    registerMessage("ops.ntos_convert_fail",     Lang::AZ, "{0} dəyəri sətirə çevrilə bilmir");
    registerMessage("ops.ntos_not_number",       Lang::EN, "Value is not a number, cannot convert to string");
    registerMessage("ops.ntos_not_number",       Lang::AZ, "Dəyər nömrə deyil, sətirə çevrilə bilməz");
    registerMessage("ops.compile_structure",     Lang::EN, "compile() received an object with an invalid structure");
    registerMessage("ops.compile_structure",     Lang::AZ, "compile() düzgün olmayan quruluşda obyekt aldı");
    registerMessage("ops.compile_wrong_arg",     Lang::EN, "compile() expects an object or a list of objects");
    registerMessage("ops.compile_wrong_arg",     Lang::AZ, "compile() obyekt və ya obyektlər siyahısı gözləyir");
    registerMessage("ops.compile_not_string",    Lang::EN, "compile(): compiler_path, out_dir and flag must be strings");
    registerMessage("ops.compile_not_string",    Lang::AZ, "compile(): compiler_path, out_dir və flag sətir olmalıdır");
    registerMessage("ops.compile_not_list",      Lang::EN, "compile(): src and tracked_src must be lists");
    registerMessage("ops.compile_not_list",      Lang::AZ, "compile(): src və tracked_src list olmalıdır");
    registerMessage("ops.compile_src_not_string", Lang::EN, "compile(): src and tracked_src must contain only strings");
    registerMessage("ops.compile_src_not_string", Lang::AZ, "compile(): src və tracked_src yalnız sətirlərdən ibarət olmalıdır");
    registerMessage("ops.compile_fail",          Lang::EN, "Failed to compile {0} (status: {1})");
    registerMessage("ops.compile_fail",          Lang::AZ, "{0} faylı kompilyasiya oluna bilmədi (status: {1})");
    registerMessage("ops.compile_success",       Lang::EN, "Compiled {0} (status: {1})");
    registerMessage("ops.compile_success",       Lang::AZ, "{0} kompilyasiya olundu (status: {1})");
    registerMessage("ops.link_wrong_args",       Lang::EN, "link() expects 3 arguments (objects list, executable name, compiler path)");
    registerMessage("ops.link_wrong_args",       Lang::AZ, "link() 3 arqument gözləyir (obyektlər siyahısı, icra adı, kompilyator yolu)");
    registerMessage("ops.link_not_list",         Lang::EN, "link(): first argument must be a list of object file paths");
    registerMessage("ops.link_not_list",         Lang::AZ, "link(): ilk arqument obyekt fayl yolları siyahısı olmalıdır");
    registerMessage("ops.link_not_string",       Lang::EN, "link(): second argument must be a string (executable name)");
    registerMessage("ops.link_not_string",       Lang::AZ, "link(): ikinci arqument sətir olmalıdır (icra adı)");
    registerMessage("ops.link_compiler_not_string", Lang::EN, "link(): third argument must be a string (compiler path)");
    registerMessage("ops.link_compiler_not_string", Lang::AZ, "link(): üçüncü arqument sətir olmalıdır (kompilyator yolu)");
    registerMessage("ops.link_fail",             Lang::EN, "Failed to link {0} (status: {1})");
    registerMessage("ops.link_fail",             Lang::AZ, "{0} link oluna bilmədi (status: {1})");
    registerMessage("ops.link_success",          Lang::EN, "Linked {0} (status: {1})");
    registerMessage("ops.link_success",          Lang::AZ, "{0} link olundu (status: {1})");
    registerMessage("ops.run_too_many",          Lang::EN, "run() accepts a single argument");
    registerMessage("ops.run_too_many",          Lang::AZ, "run() yalnız bir arqument qəbul edir");
    registerMessage("ops.run_not_string",        Lang::EN, "run() argument must be a string (script path)");
    registerMessage("ops.run_not_string",        Lang::AZ, "run() arqumenti sətir olmalıdır (skript yolu)");
    registerMessage("ops.set_include_not_string", Lang::EN, "set_include() argument must be a string");
    registerMessage("ops.set_include_not_string", Lang::AZ, "set_include() arqumenti sətir olmalıdır");

    // ---- Manager ----
    registerMessage("manager.cache_open_fail",   Lang::EN, "Could not open cache file \"{0}\"");
    registerMessage("manager.cache_open_fail",   Lang::AZ, "\"{0}\" keş faylı açıla bilmədi");
    registerMessage("manager.about_open_fail",   Lang::EN, "Could not open the 'about' file");
    registerMessage("manager.about_open_fail",   Lang::AZ, "'about' faylı açıla bilmədi");

    // ---- Env ----
    registerMessage("env.declare_duplicate",     Lang::EN, "Variable \"{0}\" is already declared");
    registerMessage("env.declare_duplicate",     Lang::AZ, "\"{0}\" dəyişəni artıq təyin olunub");
    registerMessage("env.assign_const",          Lang::EN, "Cannot assign to constant \"{0}\"");
    registerMessage("env.assign_const",          Lang::AZ, "Konstant dəyişənə təyinat oluna bilməz - {0}");
    registerMessage("env.resolve_not_found",     Lang::EN, "Variable \"{0}\" does not exist");
    registerMessage("env.resolve_not_found",     Lang::AZ, "\"{0}\" dəyişəni mövcud deyil");

    // ---- Debug system itself ----
    registerMessage("debug.unknown_lang",        Lang::EN, "Unknown language \"{0}\", defaulting to English");
    registerMessage("debug.unknown_lang",        Lang::AZ, "Bilinməyən dil \"{0}\", ingilis dili təyin edilir");
    registerMessage("debug.invalid_level",       Lang::EN, "Unknown debug level \"{0}\"");
    registerMessage("debug.invalid_level",       Lang::AZ, "Bilinməyən debug səviyyəsi \"{0}\"");
    registerMessage("debug.invalid_category",    Lang::EN, "Unknown debug category \"{0}\"");
    registerMessage("debug.invalid_category",    Lang::AZ, "Bilinməyən debug kateqoriyası \"{0}\"");
    registerMessage("debug.dsl_log",             Lang::EN, "{0}");
    registerMessage("debug.dsl_log",             Lang::AZ, "{0}");
}

} // namespace debug