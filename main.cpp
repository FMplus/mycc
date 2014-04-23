#include <iostream>
#include "Character_table.h"
#include "grammer.h"
#include <cstring>
#include <ctype.h>
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

int main()
{
    parser *p = new parser;
    int t = p->do_parse();
    if ( t == 0)
    {
        cout << "ok" << endl;
    }
    /*int t = p -> getc();
    while (t != -1 && t != -2)
    {
        t = p -> getc();
    }
    delete p;*/
    return 0;
}
