#ifndef LEXER
#define LEXER
#include <iostream>
#include <vector>
using namespace std;

const vector <string> Keywords = {
    "if", "while", "else"
};

enum class TokenType{
    Identifier, Keyword, Number, Operator, String, EndOfFile, Invalid
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
