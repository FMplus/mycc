#include "Character_table.h"
#include "token_scan.h"
#include <cstring>
#include <ctype.h>
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#if 1
using namespace std;
char buffer[100];
int token_scan(FILE *fp)
{
    char ch;
    while (!feof(fp))//is the file end;
    {
        ch = getc(fp);//get a char
        int i = 0;
        while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
        {
            ch = getc(fp);
            if (feof(fp))
                return 1;//file end return 1;
        }
        if (isalpha(ch))//get a keyword or id;  is a ~ Z;
        {
            buffer[i] = ch;i++;
            ch = getc(fp);
            while (isalnum(ch))
            {
                buffer[i] = ch;i++;
                ch = getc(fp);
            }
            ungetc(ch,fp);
            buffer[i] = '\0';
            if (strcmp("if",buffer)==0)return (IF);
            else if (strcmp("else",buffer)==0)return (ELSE);
            else if (strcmp("int",buffer)==0)return (BASIC);
            else if (strcmp("for",buffer)==0)return (FOR);
            else if (strcmp("while",buffer)==0)return (WHILE);
            //else if (strcmp("return",buffer)==0)return (RETURN);
            else return ID;
        }
        else if (isdigit(ch))//is 0~9;
        {
            buffer[i] = ch;i++;
            ch = getc(fp);
            while (isdigit(ch))
            {
                buffer[i] = ch;i++;
                ch = getc(fp);
            }
            ungetc(ch,fp);
            buffer[i] = '\0';
            return NUM;
        }
        else
        {
            switch(ch)
            {
            case '*':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (MULT);break;
            case '/':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (DEV);break;
            case '+':
                buffer[i] = ch;i++;
                ch = getc(fp);
                if (ch == '+')
                {
                    buffer[i] = ch;i++;buffer[i] = '\0';
                    return (INC);
                }
                else
                {
                    ungetc(ch,fp);
                    buffer[i] = '\0';
                    return (PLUS);
                }break;
            case '-':
                buffer[i] = ch;i++;
                ch = getc(fp);
                if (ch == '-')
                {
                    buffer[i] = ch;i++;buffer[i] = '\0';
                    return (DEC);
                }
                else
                {
                    ungetc(ch,fp);
                    buffer[i] = '\0';
                    return (MIN);
                }break;
            case '<':
                buffer[i] = ch;i++;
                ch = getc(fp);
                if (ch == '=')
                {
                    buffer[i] = ch;i++;buffer[i] = '\0';
                    return (LE);
                }
                else
                {
                    ungetc(ch,fp);
                    buffer[i] = '\0';
                    return (LT);
                }break;
            case '>':
                buffer[i] = ch;i++;
                ch = getc(fp);
                if (ch == '=')
                {
                    buffer[i] = ch;i++;buffer[i] = '\0';
                    return (GE);
                }
                else
                {
                    ungetc(ch,fp);
                    buffer[i] = '\0';
                    return (GT);
                }break;
            case '[':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (LSB);break;
            case ']':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (RSB);break;
            case '(':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (LBT);break;
            case ')':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (RBT);break;
            case '{':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (LB);break;
            case '}':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (RB);break;
            case '=':
                buffer[i] = ch;i++;
                ch = getc(fp);
                if (ch == '=')
                {
                    buffer[i] = ch;i++;buffer[i] = '\0';
                    return (ASS);break;
                }
                else
                {
                    ungetc(ch,fp);
                    buffer[i] = '\0';
                    return (EQUAL);
                }break;
            case '!':
                buffer[i] = ch;i++;
                ch = getc(fp);
                if ( ch == '=' )
                {
                    buffer[i] = ch;i++;buffer[i] = '\0';
                    return (UEQUAL);
                }
                else
                {
                    ungetc(ch,fp);
                    buffer[i] = '\0';
                    return (EXCLAMA);
                }break;
            case ',':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (COMMA);break;
            case ';':
                buffer[i] = ch;i++;buffer[i] = '\0';
                return (SEM);break;
            case '"':
                ch = getc(fp);
                while (ch != '"')
                {
                    buffer[i] = ch;
                    i++;
                }
                buffer[i] = '\0';
                return (ID);break;//actually it is STRING
            default:
                cout << "ERROR !" << endl;
                return 0;
                break;
            }
        }
    }
    return 1;
}
#endif
