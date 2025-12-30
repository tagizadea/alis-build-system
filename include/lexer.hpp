#ifndef LEXER
#define LEXER
#include <iostream>
#include <vector>
#include <map>
using namespace std;

enum class TokenType{
    Identifier, ASSIGN, IF, ELSE, WHILE, Number, PLUS, MINUS, UNARY_PLUS, UNARY_MINUS, 
    Let, Const, SEMICOLON, COLON, COMMA, DOT, BREAK, CONTINUE, FOR,
    PLUS_ASSIGN, MINUS_ASSIGN, MULT_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
    AND, OR, NOT, XOR, KICIK, BOYUK, EQUAL, NEQUAL, B_KICIK, B_BOYUK, FN,
    MULTIPLY, DIVIDE, MOD, LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, String, EndOfFile, Invalid
};

extern map <string, TokenType> Keywords;
extern map <TokenType, string> rKeywords;

struct Token{
    TokenType type;
    string value;
};

class Lexer{
public:
    Lexer(string source);

    Token* tokenize();

private:
    Token parseIdentifier();
    Token parseNumber();
    Token parseString();
    Token parseOperator();
    
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
    bool isSpace(char c);

    string source;
    int position;
};
#endif // LEXER
