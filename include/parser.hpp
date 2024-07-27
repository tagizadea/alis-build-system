#ifndef PARSER
#define PARSER
#include <lexer.hpp>
#include <stack>


struct Rule {
    std::string lhs;
    std::vector<TokenType> rhs;
};


const std::vector<Rule> grammar = {
    {"Expression", {TokenType::Identifier, TokenType::ASSIGN, TokenType::Identifier}},
    {"Expression", {TokenType::Identifier}}
};

// Shift-Reduce Parser
class Parser{
    private:

    stack <Token> st;

    bool reduce(std::stack<TokenType>& stack, std::stack<std::string>& symbols);

    public:

    bool parse(Token *tokens);
};

#endif // PARSER