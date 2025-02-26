#ifndef EVAL
#define EVAL

#include <valtypes.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <iostream>
#include <env.hpp>

std::shared_ptr <Value> evaluate(std::shared_ptr<Stmt> astNode, std::shared_ptr <Env> env_instance);

#endif