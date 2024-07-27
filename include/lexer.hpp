#ifndef LEXER
#define LEXER
#include <iostream>
#include <vector>
using namespace std;

enum class TokenType{
    Identifier, ASSIGN, IF, ELSE, WHILE, Number, PLUS, MINUS, 
    MULTIPLY, DIVIDE, MOD, String, EndOfFile, Invalid
};

const vector <pair <string, TokenType> > Keywords = {
    {"if", TokenType::IF},
    {"while", TokenType::WHILE},
    {"else", TokenType::ELSE}
};

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
