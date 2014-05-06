#ifndef _NODE_H_
#define _NODE_H_
#include <iostream>
#include <stdlib.h>
#include <stack>
#include <vector>

struct node
{
    typedef std::string string;
    string code;
    string res;
    bool isID;
    bool isNUM;
    bool isREG;
    bool isREGID;
};

extern std::stack<node> nodelist;

#endif
