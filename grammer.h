#ifndef _GRAMMER_H
#define _GRAMMER_H

#include<stack>
#include "Character_table.h"
#include "token_scan.h"
#define  REDUCE     0
#define  SHIFT      1
#define  ERROR      3

class parser
{
    public:
        static const int cctab[495][47][2];
        FILE *fp;
        parser()
        {
            fp = fopen("test.c","r");
        }
        ~parser()
        {
            fclose(fp);
        }
        int getc();
        int do_parse();
};//class parser

#endif // _GRAMMER_H
