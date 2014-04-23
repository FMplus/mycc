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

#define  program    2
#define  block      3
#define  decls      4
#define  decl       5
#define  id         6
#define  stmts      7
#define  stmt       8
#define  loc        9
#define  expr       10
#define  equality   11
#define  rel        12
#define  add        13
#define  mult       14
#define  unary      15
#define  postfix    16
#define  factor     17
#define  BASIC      18  //int
#define  LSB        19  // [
#define  RSB        20  // ]
#define  LB         21  // {
#define  RB         22  // }
#define  LBT        23  // (
#define  RBT        24  // )
#define  COMMA      25  // ,
#define  SEM        26  // ;
#define  PLUS       27  // +
#define  MIN        28  // -
#define  MULT       29  // *
#define  DEV        30  // /
#define  EQUAL      31  // =
#define  UEQUAL     32  // !=
#define  EXCLAMA    33  // !
#define  ASS        34  // ==
#define  INC        35  // ++
#define  DEC        36  // --
#define  GT         37  // >
#define  GE         38  // >=
#define  LT         39  // <
#define  LE         40  // <=
#define  ID         41  // id
#define  NUM        42  // num
#define  IF         43  // if
#define  FOR        44  // for
#define  WHILE      45  // while
#define  ELSE       46  // else

#endif // CHARACTER_TABLE_H_INCLUDED
