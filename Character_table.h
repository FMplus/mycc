//#ifndef _CHARACTER_TABLE_H
//#define _CHARACTER_TABLE_H
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <list>
#include <numeric>
#include <algorithm>
#include <cstdio>
#include <ctype.h>

#if 1//symbols define
//symbol of val
#define    program      2
#define    block        3
#define    decls        4
#define    decl         5
#define    id           6
#define    stmts        7
#define    stmt         8
#define    loc          9
#define    expr         10
#define    volua        11
#define    stmvol       12
#define    equality     13
#define    rel          14
#define    add          15
#define    mult         16
#define    unary        17
#define    postfix      18
#define    factor       19
#define    paramer      20
//symbol of term
//sign
#define    BASIC        21  // int (basic type)
#define    LSB          22  // [
#define    RSB          23  // ]
#define    LB           24  // {
#define    RB           25  // }
#define    LBT          26  // (
#define    RBT          27  // )
#define    COMMA        28  // ,
#define    SEM          29  // ;
#define    PLUS         30  // +
#define    MIN          31  // -
#define    MULT         32  // *
#define    DEV          33  // /
#define    EQUAL        34  // =
#define    UEQUAL       35  // !=
#define    EXCLAMA      36  // !
#define    ASS          37  // ==
#define    INC          38  // ++
#define    DEC          39  // --
#define    GT           40  // >
#define    GE           41  // >=
#define    LT           42  // <
#define    LE           43  // <=
//value
#define    ID           44  // id
#define    NUM          45  // num
//key words
#define    IF           46  // if
#define    FOR          47  // for
#define    WHILE        48  // while
#define    ELSE         49  // else
#define    PRINTF       50  // printf
#define    STR          51  // str
#define    RETURN       52  // return

#endif // CHARACTER_TABLE_H_INCLUDED


