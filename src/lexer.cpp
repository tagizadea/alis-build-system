#include <lexer.hpp>

// Lexer implementation
Lexer::Lexer(string source) : source(source), position(0) {}

Token* Lexer::tokenize(){
    int maxTokens = 1000; // Arbitrary large number for simplicity
    Token* tokens = new Token[maxTokens];
    int tokenIndex = 0;

    while(source[position] != '\0'){
        if(isSpace(source[position])) position++;
        else if(isAlpha(source[position])) tokens[tokenIndex++] = parseIdentifier();
        else if(isDigit(source[position])) tokens[tokenIndex++] = parseNumber();
        else if(source[position] == '\"') tokens[tokenIndex++] = parseString();
        else tokens[tokenIndex++] = parseOperator();
    }
    tokens[tokenIndex++] = {TokenType::EndOfFile, ""};
    return tokens;
}

Token Lexer::parseIdentifier(){
    string start = "";
    start += source[position++];
    while(isAlphaNumeric(source[position])){
        start += source[position];
        position++;
    }

    for(string s : Keywords){
        if(s == start) return {TokenType::Keyword, start};
    }

    return {TokenType::Identifier, start};
}

Token Lexer::parseNumber(){
    string start = "";
    start += source[position++];
    while(isDigit(source[position])){
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
    if(source[position] == '\"'){
        start += source[position];
        position++;
    } // Skip the closing quote
    return {TokenType::String, start};
}

Token Lexer::parseOperator(){
    string start = "";
    start += source[position++];
    return {TokenType::Operator, start};
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