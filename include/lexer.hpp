#ifndef LEXER
#define LEXER
#include <iostream>
#include <vector>
#include <map>

enum class TokenType{
    Identifier, ASSIGN, IF, ELSE, WHILE, Number, PLUS, MINUS, UNARY_PLUS, UNARY_MINUS, 
    Let, Const, SEMICOLON, COLON, COMMA, DOT, BREAK, CONTINUE, FOR,
    PLUS_ASSIGN, MINUS_ASSIGN, MULT_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
    AND, OR, NOT, XOR, KICIK, BOYUK, EQUAL, NEQUAL, B_KICIK, B_BOYUK, FN,
    MULTIPLY, DIVIDE, MOD, LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, String, EndOfFile, Invalid
};

extern std::map <std::string, TokenType> Keywords;
extern std::map <TokenType, std::string> rKeywords;

struct Token{
    TokenType type;
    std::string value;
    int line = 1;
    int col = 1;
};

class Lexer{
public:
    Lexer(std::string source);

    std::vector<Token> tokenize();

private:
    Token parseIdentifier();
    Token parseNumber();
    Token parseString();
    Token parseOperator();
    
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
    bool isSpace(char c);

    std::string source;
    int position;
    int line = 1;
    int col = 1;
};
#endif // LEXER
