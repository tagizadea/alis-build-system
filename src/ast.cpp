#include <ast.hpp>

NumericLiteral::NumericLiteral(std::string val){
    kind = NodeType::NUMERIC_L;
    try{
        this->val = std::stold(val);
    }
    catch(const std::exception& e){
        std::cerr << "TEMP_OUT_numericlclass: ." << val << ".\n"; // temp
        std::cerr << "Error: " << e.what() << '\n';
    }
}

Program::Program(){
    kind = NodeType::PROGRAM;
}

