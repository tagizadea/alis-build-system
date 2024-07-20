#ifndef LEXER
#define LEXER

enum class TokenType {
    Identifier, Keyword, Number, Operator, String, EndOfFile, Invalid
};

struct Token {
    TokenType type;
    const char* value;
    int length;
};

class Lexer {
public:
    Lexer(const char* source);

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

    const char* source;
    int position;
};
#endif // LEXER
