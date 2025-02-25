#ifndef EVAL
#define EVAL

#include <valtypes.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <iostream>

Value* evaluate(Stmt* astNode);

#endif