#ifndef OPERATIONS
#define OPERATIONS
#include <iostream>
#include <vector>
#include <map>
using namespace std;

vector <string> getSystemFiles(vector <string> &files);

void hello();

typedef void (*FuncPtr)();
extern std::map<std::string, FuncPtr> functionMap;

#endif