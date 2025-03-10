#include <lexer.hpp>

map <string, TokenType> Keywords = {
    {"if", TokenType::IF},
    {"while", TokenType::WHILE},
    {"else", TokenType::ELSE},
    {"Identifier", TokenType::Identifier},
    {"let", TokenType::Let},
    {"const", TokenType::Const},
    {"fn", TokenType::FN},
    {"=", TokenType::ASSIGN},
    {"Number", TokenType::Number},
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::MULTIPLY},
    {"/", TokenType::DIVIDE},
    {"%", TokenType::MOD},
    {"&&", TokenType::AND},
    {"||", TokenType::OR},
    {"==", TokenType::EQUAL},
    {"!", TokenType::NOT},
    {"!=", TokenType::NEQUAL},
    {"^", TokenType::XOR},
    {"<", TokenType::KICIK},
    {">", TokenType::BOYUK},
    {"<=", TokenType::B_KICIK},
    {">=", TokenType::B_BOYUK},
    {"(", TokenType::LPAREN},
    {")", TokenType::RPAREN},
    {"[", TokenType::LBRACE},
    {"]", TokenType::RBRACE},
    {"{", TokenType::LBRACK},
    {"}", TokenType::RBRACK},
    {";", TokenType::SEMICOLON},
    {":", TokenType::COLON},
    {",", TokenType::COMMA},
    {".", TokenType::DOT},
    {"String", TokenType::String},
    {"Invalid", TokenType::Invalid}
};

map <TokenType, string> rKeywords;

Lexer::Lexer(string source) : source(source), position(0) {}

Token* Lexer::tokenize(){
    int maxTokens = 2000000;
    Token* tokens = new Token[maxTokens];
    int tokenIndex = 0;

    while(source[position] != '\0'){
        if(isSpace(source[position])) position++;
        else if(isAlpha(source[position])) tokens[tokenIndex++] = parseIdentifier();
        else if(isDigit(source[position])) tokens[tokenIndex++] = parseNumber();
        else if(source[position] == '\"') tokens[tokenIndex++] = parseString();
        else if(source[position] == '#') while(source[position++] != '\n');
        else tokens[tokenIndex++] = parseOperator();
    }
    tokens[tokenIndex++] = {TokenType::EndOfFile, ""};

    for(pair <string, TokenType> token : Keywords)
        rKeywords[token.second] = token.first;

    return tokens;
}

Token Lexer::parseIdentifier(){
    string start = "";
    start += source[position++];
    while(isAlphaNumeric(source[position])){
        start += source[position];
        position++;
    }

    /*for(pair <string, TokenType> s : Keywords){
        if(s.first == start) return {s.second, start};
    }*/
    if(Keywords.find(start) != Keywords.end())
        return {Keywords.find(start)->second, start};

    return {TokenType::Identifier, start};
}

Token Lexer::parseNumber(){
    string start = "";
    start += source[position++];
    while(isDigit(source[position]) || source[position] == '.'){
        start += source[position];
        position++;
    }
    return {TokenType::Number, start};
}

Token Lexer::parseString(){
    position++; // Skip the opening quote
    string start = "";
    start += source[position++];
    while(source[position] != '\"' && source[position] != '\0'){
        start += source[position];
        position++;
    }
    position++; // Skip the closing quote
    return {TokenType::String, start};
}

Token Lexer::parseOperator(){
    string start = "";
    start += source[position++];

    while(source[position] == '&' || source[position] == '|' || 
          source[position] == '=' || source[position] == '!' ||
          source[position] == '>' || source[position] == '<'){
            if(source[position - 1] == '(' || source[position - 1] == ')') break;
            start += source[position++];
          }
    
    if(Keywords.find(start) != Keywords.end()) return {Keywords[start], start};
    else return {TokenType::Invalid, start};
}

bool Lexer::isAlpha(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c){
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c){
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isSpace(char c){
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}