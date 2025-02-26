#ifndef EVAL
#define EVAL

#include <valtypes.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <iostream>
#include <env.hpp>

Value* evaluate(Stmt* astNode, Env* env_instance);

#endif