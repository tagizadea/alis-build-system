#include <parser.hpp>
#include <cstring>
#include <stdio.h>

// Lexer implementation
Lexer::Lexer(const char* source) : source(source), position(0) {}

Token* Lexer::tokenize() {
    int maxTokens = 1000; // Arbitrary large number for simplicity
    Token* tokens = new Token[maxTokens];
    int tokenIndex = 0;

    while (source[position] != '\0') {
        if (isSpace(source[position])) {
            position++;
        } else if (isAlpha(source[position])) {
            tokens[tokenIndex++] = parseIdentifier();
        } else if (isDigit(source[position])) {
            tokens[tokenIndex++] = parseNumber();
        } else if (source[position] == '\"') {
            tokens[tokenIndex++] = parseString();
        } else {
            tokens[tokenIndex++] = parseOperator();
        }
    }
    tokens[tokenIndex++] = {TokenType::EndOfFile, "", 0};
    return tokens;
}

Token Lexer::parseIdentifier() {
    const char* start = source + position;
    while (isAlphaNumeric(source[position])) {
        position++;
    }
    return {TokenType::Identifier, start, int(source + position - start)};
}

Token Lexer::parseNumber() {
    const char* start = source + position;
    while (isDigit(source[position])) {
        position++;
    }
    return {TokenType::Number, start, int(source + position - start)};
}

Token Lexer::parseString() {
    position++; // Skip the opening quote
    const char* start = source + position;
    while (source[position] != '\"' && source[position] != '\0') {
        position++;
    }
    if (source[position] == '\"') {
        position++; // Skip the closing quote
    }
    return {TokenType::String, start, int(source + position - start - 1)};
}

Token Lexer::parseOperator() {
    const char* start = source + position;
    position++;
    return {TokenType::Operator, start, 1};
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Parser implementation
Parser::Parser(Token* tokens) : tokens(tokens), position(0) {}

void Parser::parse() {
    while (tokens[position].type != TokenType::EndOfFile) {
        if (tokens[position].type == TokenType::Keyword) {
            parseKeyword();
        } else if (tokens[position].type == TokenType::Identifier) {
            parseIdentifier();
        } else {
            parseExpression();
        }
    }
}

void Parser::parseKeyword() {
    if (match("func")) {
        parseFunction();
    } else {
        throwError("Unknown keyword");
    }
}

void Parser::parseIdentifier() {
    Token idToken = tokens[position++];
    if (match("=")) {
        parseExpression();
    } else if (match("(")) {
        parseFunctionCall();
    } else {
        throwError("Unexpected token after identifier");
    }
}

void Parser::parseFunction() {
    if (!expect(TokenType::Identifier)) throwError("Expected function name");
    if (!expect("(")) throwError("Expected '(' after function name");
    while (!match(")")) {
        if (tokens[position].type != TokenType::Identifier) throwError("Expected parameter name");
        position++;
        if (!match(",")) break;
    }
    if (!expect(")")) throwError("Expected ')' after parameters");
    parseBlock();
}

void Parser::parseFunctionCall() {
    while (!match(")")) {
        parseExpression();
        if (!match(",")) break;
    }
    if (!expect(")")) throwError("Expected ')' after arguments");
}

void Parser::parseBlock() {
    if (!expect("{")) throwError("Expected '{' to start block");
    while (!match("}")) {
        parse();
    }
}

void Parser::parseExpression() {
    parseTerm();
    while (match("+") || match("-")) {
        nextToken();
        parseTerm();
    }
}

void Parser::parseTerm() {
    parseFactor();
    while (match("*") || match("/")) {
        nextToken();
        parseFactor();
    }
}

void Parser::parseFactor() {
    if (tokens[position].type == TokenType::Number) {
        nextToken();
    } else if (tokens[position].type == TokenType::String) {
        nextToken();
    } else if (tokens[position].type == TokenType::Identifier) {
        nextToken();
    } else if (match("(")) {
        nextToken();
        parseExpression();
        if (!expect(")")) throwError("Expected ')'");
    } else {
        throwError("Unexpected token in factor");
    }
}

bool Parser::match(const char* op) {
    if (tokens[position].type == TokenType::Operator && strncmp(tokens[position].value, op, tokens[position].length) == 0) {
        return true;
    }
    return false;
}

bool Parser::expect(TokenType type) {
    if (tokens[position].type == type) {
        nextToken();
        return true;
    }
    return false;
}

bool Parser::expect(const char* op) {
    if (match(op)) {
        nextToken();
        return true;
    }
    return false;
}

void Parser::nextToken() {
    if (tokens[position].type != TokenType::EndOfFile) {
        position++;
    }
}

void Parser::throwError(const char* message) {
    // Basic error handling, print message and exit
    print(message);
    while (true);
}

void Parser::print(const char* message) {
    // Minimal print function
    for (const char* p = message; *p; p++) {
        putchar(*p);
    }
    putchar('\n');
}
