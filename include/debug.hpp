#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <cctype>
#include <type_traits>
#include <utility>

// ============================================================================
//  Compile-time configuration
// ============================================================================
//
//  Default language for messages. Define at compile time:
//      g++ -DLANG=EN   (default / English)
//      g++ -DLANG=AZ   (Azerbaijani)
//  If not defined, defaults to English. The runtime language can be changed
//  later from the ABS scripting language via set_lang("AZ").
//
#ifndef LANG
    #define LANG "EN"
#endif

// Optional compile-time severity ceiling. Values match debug::Level:
//   0=Trace 1=Debug 2=Info 3=Warning 4=Error 5=Fatal
// If ABS_MAX_SEVERITY is defined, everything above it compiles to nothing.
// #define ABS_MAX_SEVERITY 4

// Feature toggles (off by default for zero overhead):
// #define ABS_PROFILE    1   // enable performance profiling
// #define ABS_MEM_TRACK  1   // enable memory allocation tracking

// ============================================================================
//  core debug namespace
// ============================================================================

namespace debug {

// ---------------------------------------------------------------------------
//  Level & Category enums
// ---------------------------------------------------------------------------

enum class Level : int {
    Trace   = 0,
    Debug   = 1,
    Info    = 2,
    Warning = 3,
    Error   = 4,
    Fatal   = 5
};

enum class Category : int {
    General    = 0,
    Lexer      = 1,
    Parser     = 2,
    Eval       = 3,
    Env        = 4,
    Manager    = 5,
    Operations = 6,
    Memory     = 7,
    Performance= 8,
    DSL        = 9
};

enum class Lang : int {
    EN = 0,
    AZ = 1
};

// ---------------------------------------------------------------------------
//  string helpers (OS-independent)
// ---------------------------------------------------------------------------

namespace detail {

// Convert a value to string for message formatting.
template <typename T>
inline std::string toStr(const T& v){
    if constexpr (std::is_same_v<T, std::string>) return v;
    else if constexpr (std::is_same_v<T, const char*>) return std::string(v);
    else if constexpr (std::is_same_v<T, char*>) return std::string(v);
    else if constexpr (std::is_same_v<T, bool>) return v ? "true" : "false";
    else if constexpr (std::is_same_v<T, char>) return std::string(1, v);
    else {
        std::ostringstream oss;
        oss << v;
        return oss.str();
    }
}

// Replace {0}, {1}, ... placeholders with parameters.
inline std::string formatImpl(const std::string& fmt, const std::vector<std::string>& params){
    std::string result;
    result.reserve(fmt.size() + 32);
    for(size_t i = 0; i < fmt.size(); ++i){
        if(fmt[i] == '{'){
            size_t end = fmt.find('}', i);
            if(end != std::string::npos){
                std::string num = fmt.substr(i + 1, end - i - 1);
                bool allDigits = !num.empty();
                for(char c : num) if(!std::isdigit(static_cast<unsigned char>(c))){ allDigits = false; break; }
                if(allDigits){
                    int idx = std::stoi(num);
                    if(idx >= 0 && idx < static_cast<int>(params.size())){
                        result += params[idx];
                        i = end;
                        continue;
                    }
                }
            }
        }
        result += fmt[i];
    }
    return result;
}

} // namespace detail

// Variadic formatter: format("Value: {0} and {1}", a, b)
template <typename... Args>
inline std::string format(const std::string& fmt, Args&&... args){
    std::vector<std::string> params;
    params.reserve(sizeof...(args));
    (params.push_back(detail::toStr(std::forward<Args>(args))), ...);
    return detail::formatImpl(fmt, params);
}

// ---------------------------------------------------------------------------
//  Debugger singleton
// ---------------------------------------------------------------------------

class Debugger {
public:
    static Debugger& getInstance();

    // Language: compile-time default from LANG, changeable at runtime.
    Lang getLanguage() const;
    void setLanguage(Lang lang);
    static Lang langFromString(const std::string& s);

    // Severity control.
    void setLevel(Level lvl);                 // global minimal level
    void setCategoryLevel(Category cat, Level lvl);
    bool isEnabled(Level lvl, Category cat) const;

    // Output control.
    void setOutputStream(std::ostream& os);
    void setUseColor(bool enabled);

    // Script context (for .abs error reporting).
    void setScriptContext(const std::string& path);  // push script path
    void popScriptContext();                          // pop script path (nested run())
    void setScriptLine(int line);
    void setScriptCol(int col);
    std::string getScriptPath() const;

    // Core log call.
    void log(Level lvl, Category cat, const char* file, int line,
             const char* func, const std::string& msgId, const std::string& msg);

    // Message catalog lookup. Returns the formatted-with-language template.
    std::string getTemplate(const std::string& msgId) const;

    // Register a message template for a language.
    void registerMessage(const std::string& id, Lang lang, std::string fmt);

    // ---- Profiling (opt-in via ABS_PROFILE) ----
    void profileStart(const char* name);
    void profileEnd(const char* name, std::chrono::steady_clock::duration dur);
    void dumpProfile();

    // ---- Memory tracking (opt-in via ABS_MEM_TRACK) ----
    void memAlloc(std::size_t bytes);
    void memFree(std::size_t bytes);
    void dumpMemory();

private:
    Debugger();
    ~Debugger() = default;
    Debugger(const Debugger&) = delete;
    Debugger& operator=(const Debugger&) = delete;

    struct Impl;
    Impl* impl_;
};

// ---------------------------------------------------------------------------
//  Message registration helper (used internally by catalogs)
// ---------------------------------------------------------------------------

void registerMessage(const std::string& id, Lang lang, std::string fmt);

// ---------------------------------------------------------------------------
//  Scope timer for profiling
// ---------------------------------------------------------------------------

class ScopeTimer {
public:
    explicit ScopeTimer(const char* name);
    ~ScopeTimer();
private:
    const char* name_;
    std::chrono::steady_clock::time_point start_;
};

// ---------------------------------------------------------------------------
//  Logging macros
// ---------------------------------------------------------------------------

namespace detail {
template <typename... Args>
inline void log_msg(Level lvl, Category cat, const char* file, int line,
                    const char* func, const std::string& msgId, Args&&... args){
    Debugger& dbg = Debugger::getInstance();
    if(!dbg.isEnabled(lvl, cat)) return;
    std::string fmt = dbg.getTemplate(msgId);
    std::string msg = format(fmt, std::forward<Args>(args)...);
    dbg.log(lvl, cat, file, line, func, msgId, msg);
}
} // namespace detail

#if defined(ABS_MAX_SEVERITY)
    #define ABS_TRACE(category, id, ...)    do { if constexpr (debug::Level::Trace   <= static_cast<debug::Level>(ABS_MAX_SEVERITY)) { debug::detail::log_msg(debug::Level::Trace,   category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); } } while(0)
    #define ABS_DEBUG(category, id, ...)    do { if constexpr (debug::Level::Debug   <= static_cast<debug::Level>(ABS_MAX_SEVERITY)) { debug::detail::log_msg(debug::Level::Debug,   category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); } } while(0)
    #define ABS_INFO(category, id, ...)     do { if constexpr (debug::Level::Info    <= static_cast<debug::Level>(ABS_MAX_SEVERITY)) { debug::detail::log_msg(debug::Level::Info,    category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); } } while(0)
    #define ABS_WARNING(category, id, ...)  do { if constexpr (debug::Level::Warning <= static_cast<debug::Level>(ABS_MAX_SEVERITY)) { debug::detail::log_msg(debug::Level::Warning, category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); } } while(0)
    #define ABS_ERROR(category, id, ...)    do { if constexpr (debug::Level::Error   <= static_cast<debug::Level>(ABS_MAX_SEVERITY)) { debug::detail::log_msg(debug::Level::Error,   category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); } } while(0)
    #define ABS_FATAL(category, id, ...)    do { if constexpr (debug::Level::Fatal   <= static_cast<debug::Level>(ABS_MAX_SEVERITY)) { debug::detail::log_msg(debug::Level::Fatal,   category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); std::abort(); } } while(0)
#else
    #define ABS_TRACE(category, id, ...)    debug::detail::log_msg(debug::Level::Trace,   category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__)
    #define ABS_DEBUG(category, id, ...)    debug::detail::log_msg(debug::Level::Debug,   category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__)
    #define ABS_INFO(category, id, ...)     debug::detail::log_msg(debug::Level::Info,    category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__)
    #define ABS_WARNING(category, id, ...)  debug::detail::log_msg(debug::Level::Warning, category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__)
    #define ABS_ERROR(category, id, ...)    debug::detail::log_msg(debug::Level::Error,   category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__)
    #define ABS_FATAL(category, id, ...)    do { debug::detail::log_msg(debug::Level::Fatal, category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); std::abort(); } while(0)
#endif

// Assertion: logs and aborts on failure. Condition is ALWAYS evaluated.
#define ABS_ASSERT(cond, category, id, ...) \
    do { if(!(cond)){ debug::detail::log_msg(debug::Level::Fatal, category, __FILE__, __LINE__, __func__, id, ##__VA_ARGS__); std::abort(); } } while(0)

// ---------------------------------------------------------------------------
//  Profiling macros
// ---------------------------------------------------------------------------

#if defined(ABS_PROFILE) && ABS_PROFILE
    #define ABS_PROFILE_SCOPE(name) debug::ScopeTimer abs_profile_timer_##__LINE__(name)
    #define ABS_PROFILE_FUNC()      debug::ScopeTimer abs_profile_timer_##__LINE__(__func__)
#else
    #define ABS_PROFILE_SCOPE(name) ((void)0)
    #define ABS_PROFILE_FUNC()      ((void)0)
#endif

} // namespace debug

// Convenience category constants for use in macros (global scope).
namespace cat {
    constexpr debug::Category General     = debug::Category::General;
    constexpr debug::Category Lexer       = debug::Category::Lexer;
    constexpr debug::Category Parser      = debug::Category::Parser;
    constexpr debug::Category Eval        = debug::Category::Eval;
    constexpr debug::Category Env         = debug::Category::Env;
    constexpr debug::Category Manager     = debug::Category::Manager;
    constexpr debug::Category Operations  = debug::Category::Operations;
    constexpr debug::Category Memory      = debug::Category::Memory;
    constexpr debug::Category Performance = debug::Category::Performance;
    constexpr debug::Category DSL         = debug::Category::DSL;
}

// Convenience language constants (global scope).
namespace lang {
    constexpr debug::Lang EN = debug::Lang::EN;
    constexpr debug::Lang AZ = debug::Lang::AZ;
}

#endif // DEBUG_HPP
