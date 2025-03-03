#ifndef EVAL
#define EVAL

#include <valtypes.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <iostream>
#include <env.hpp>
#include <set>

Value* evaluate(Stmt* astNode, Env* env_instance);

#endif