#include <lexer.hpp>
using namespace std;
#include <debug.hpp>

map <string, TokenType> Keywords = {
    {"if", TokenType::IF},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"else", TokenType::ELSE},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    // {"Identifier", TokenType::Identifier},
    {"let", TokenType::Let},
    {"const", TokenType::Const},
    {"fn", TokenType::FN},
    {"=", TokenType::ASSIGN},
    // {"Number", TokenType::Number},
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"+=", TokenType::PLUS_ASSIGN},
    {"-=", TokenType::MINUS_ASSIGN},
    {"*=", TokenType::MULT_ASSIGN},
    {"/=", TokenType::DIV_ASSIGN},
    {"%=", TokenType::MOD_ASSIGN},
    {"++", TokenType::UNARY_PLUS},
    {"--", TokenType::UNARY_MINUS},
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
    // {"String", TokenType::String},
    // {"Invalid", TokenType::Invalid}
};

map <TokenType, string> rKeywords;

Lexer::Lexer(string source) : source(source), position(0) {}

vector<Token> Lexer::tokenize(){
    ABS_PROFILE_FUNC();
    vector<Token> tokens;
    tokens.reserve(1024);

    while(source[position] != '\0'){
        if(isSpace(source[position])){
            if(source[position] == '\n'){
                ++line;
                col = 1;
            }
            else ++col;
            position++;
        }
        else if(isAlpha(source[position])) tokens.push_back(parseIdentifier());
        else if(isDigit(source[position])) tokens.push_back(parseNumber());
        else if(source[position] == '\"') tokens.push_back(parseString());
        else if(source[position] == '#'){
            while(source[position] != '\0' && source[position] != '\n'){
                ++col;
                position++;
            }
        }
        else tokens.push_back(parseOperator());
    }
    tokens.push_back({TokenType::EndOfFile, "", line, col});

    for(pair <string, TokenType> token : Keywords)
        rKeywords[token.second] = token.first;

    return tokens;
}

Token Lexer::parseIdentifier(){
    int startLine = line, startCol = col;
    string start = "";
    start += source[position++];
    ++col;
    while(isAlphaNumeric(source[position])){
        start += source[position];
        position++;
        ++col;
    }

    if(Keywords.find(start) != Keywords.end())
        return {Keywords.find(start)->second, start, startLine, startCol};

    return {TokenType::Identifier, start, startLine, startCol};
}

Token Lexer::parseNumber(){
    int startLine = line, startCol = col;
    string start = "";
    start += source[position++];
    ++col;
    while(isDigit(source[position]) || source[position] == '.'){
        start += source[position];
        position++;
        ++col;
    }
    return {TokenType::Number, start, startLine, startCol};
}

Token Lexer::parseString(){
    int startLine = line, startCol = col;
    position++; // Skip the opening quote
    ++col;
    string start = "";
    while(source[position] != '\"' && source[position] != '\0'){
        start += source[position];
        position++;
        ++col;
    }
    position++; // Skip the closing quote
    ++col;
    return {TokenType::String, start, startLine, startCol};
}

Token Lexer::parseOperator(){
    int startLine = line, startCol = col;
    string start = "";
    start += source[position++];
    ++col;
    if(source[position - 1] == '[' || source[position - 1] == ']' ||
    source[position - 1] == '{' || source[position - 1] == '}' ||
    source[position - 1] == '(' || source[position - 1] == ')') goto moterize;
    while(source[position] == '&' || source[position] == '|' || 
          source[position] == '=' || source[position] == '!' ||
          source[position] == '+' || source[position] == '-' ||
          source[position] == '>' || source[position] == '<'){
            if(source[position - 1] == '(' || source[position - 1] == ')') break;
            start += source[position++];
            ++col;
          }
    moterize:
    if(Keywords.find(start) != Keywords.end()) return {Keywords[start], start, startLine, startCol};
    else return {TokenType::Invalid, start, startLine, startCol};
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