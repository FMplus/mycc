#ifndef _GRAMMER_H
#define _GRAMMER_H

#include <stack>
#include <string>
#include <string.h>
#include "node.h"
#include "Character_table.h"
#include "token_scan.h"
#define  REDUCE     0
#define  SHIFT      1
#define  ERROR      3

class parser
{
    private:
        static const int cctab[495][53][2];
        FILE *fp;
    public:
        typedef std::string string;
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
        std::string transdef(int l);
        /*void change()
        {
            std::cout << cctab[468][49][0] << std::endl;
            cctab[468][49][0] = 1;
            std::cout << cctab[468][49][0] << std::endl;
            std::cout << cctab[468][49][1] << std::endl;
            cctab[468][49][1] = 486;
            std::cout << cctab[468][49][1] << std::endl;
            for (int i = 0 ; i < 495; i++)
            {
                std::cout << "{" ;
                for(int j = 0 ; j < 53 ; j++)
                {
                    if(j < 52)
                        std:: cout << "{" << cctab[i][j][0] << "," << cctab[i][j][1] << "},";
                    else
                        std:: cout << "{" << cctab[i][j][0] << "," << cctab[i][j][1] << "}";
                }
                std:: cout << "}," << std::endl;
            }
        }*/

};//class parser

#endif // _GRAMMER_H
