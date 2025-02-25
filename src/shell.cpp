#include <lexer.hpp>
#include <parser.hpp>
#include <iostream>
using namespace std;

int repl(Token* tokens){
    const Parser* parser = new Parser(tokens);
    cout << "### Repl ###\n";
    bool is_running = true;
    while(is_running){
        string input;
        cout << ">>> ";
        cin >> input;
        
    }
    return 0;
}