#include <ast.hpp>
#include <iostream>
#include <portable.hpp>

NumericLiteral::NumericLiteral(std::string val){
    kind = NodeType::NUMERIC_L;
    try{
#if ABS_32BIT
        this->val = std::stod(val);
#else
        this->val = std::stold(val);
#endif
    }
    catch(const std::exception& e){
        std::cerr << "TEMP_OUT_numericlclass: ." << val << ".\n"; // temp
        std::cerr << "NumericLiteral Error: " << e.what() << '\n';
    }
}

Program::Program(){
    kind = NodeType::PROGRAM;
}

StringLiteral::StringLiteral(std::string val){
    kind = NodeType::STRING_L;
    this->val = val;
}
