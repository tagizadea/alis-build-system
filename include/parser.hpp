#ifndef PARSER
#define PARSER
#include <lexer.hpp>
#include <stack>


struct Rule {
    string lhs;
    vector <string> rhs;
};


const std::vector<Rule> grammar = {
    {"Program", {"Program", "Statement"}},
    {"Program", {"Statement"}},
    {"Expression", {"Expression", "=", "Expression"}},
    {"Expression", {"Identifier"}},
    {"Expression", {"Number"}}
};

// Shift-Reduce Parser
class Parser{
    private:

    stack <Token> st;

    bool reduce(stack<string>& stack, std::stack<std::string>& symbols);

    public:

    bool parse(Token *tokens);
};

#endif // PARSER