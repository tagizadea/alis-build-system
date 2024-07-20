#ifndef PARSER
#define PARSER

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

class Parser {
public:
    Parser(Token* tokens);
    void parse();
    
private:
    void parseKeyword();
    void parseIdentifier();
    void parseFunction();
    void parseFunctionCall();
    void parseBlock();
    void parseExpression();
    void parseTerm();
    void parseFactor();
    bool match(const char* op);
    bool expect(TokenType type);
    bool expect(const char* op);
    void nextToken();
    void throwError(const char* message);
    void print(const char* message);

    Token* tokens;
    int position;
};

#endif // PARSER
