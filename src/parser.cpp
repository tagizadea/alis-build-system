#include <parser.hpp>
#include <iostream>
#include <vector>
using namespace std;


bool Parser::reduce(stack<string>& stack, std::stack<std::string>& symbols){
    for(const auto& rule : grammar){
        std::vector<string> rhs(rule.rhs.rbegin(), rule.rhs.rend());
        std::stack<string> temp_stack = stack;
        bool match = true;
        for(const auto& symbol : rhs){
            if(temp_stack.empty() || temp_stack.top() != symbol){
                match = false;
                break;
            }
            temp_stack.pop();
        }
        if(match){
            stack.push("Identifier");
            symbols.push(rule.lhs);
            return true;
        }
    }
    return false;
}

// Bottom-up parser
bool Parser::parse(Token *tokens) {
    std::stack<string> stack;
    std::stack<std::string> symbols;
    size_t index = 0;
    while(tokens[index].type != TokenType::EndOfFile || !stack.empty()){
        if(tokens[index].type != TokenType::EndOfFile){
            stack.push(rKeywords[tokens[index].type]);
            symbols.push(tokens[index].value);
            index++;
        }
        while(reduce(stack, symbols)){

        }
    }
    return stack.size() == 1 && stack.top() == "Identifier";
}