#include "grammer.h"
#include "signtable.h"
#include <sstream>



const std::string LABEL_NAME_PREFIX = "._L_";

std::string parser::transdef(int l)
{
    switch(l)
    {
        case 2:
            return "program";
        case 3:
            return "block";
        case 4:
            return "decls";
        case 5:
            return "decl";
        case 6:
            return "id";
        case 7:
            return "stmts";
        case 8:
            return "stmt";
        case 9:
            return "loc";
        case 10:
            return "expr";
        case 11:
            return "volua";
        case 12:
            return "stmvol";
        case 13:
            return "equality";
        case 14:
            return "rel";
        case 15:
            return "add";
        case 16:
            return "mult";
        case 17:
            return "unary";
        case 18:
            return "postfix";
        case 19:
            return "factor";
        case 20:
            return "paramer";
        default:
            std::cout << "error: not have this val!" << std::endl;
            exit(1);
    }
}

int parser::getc()
{
    #if 1
    int t = token_scan(fp);
    std::cout << t << "  " << buffer << std::endl;
    return t;
    #endif
}
int parser::do_parse()
{
    static int label_id = 3;
    int l1,l2,l3;
    std::stack<int> status;
    std::stack<string> products;
    std::stack<node> nodelist;
    SymbolTable table;
    node nodetmp,nodetmp1,nodetmp2,nodetmp3,nodetmp4;
    std::stringstream ss;
    string str,str1,str2,str3;
    int arraysize = 0;

    int sym,top;
    status.push(0);//push the start status
    products.push("\0");//push the end symbols
    top = status.top();
    sym = getc();
    while(true)
    {
        switch(cctab[top][sym][0])
        {
        case SHIFT:
        {
            products.push(buffer);
            status.push(cctab[top][sym][1]);
            sym = getc();
            top = status.top();
            break;
        }
        case REDUCE:{
        int l;
        std::cout << "case: " << cctab[top][sym][1] << std::endl;
        switch(cctab[top][sym][1]){
        case 0:{//accept
            std::cout << "accept!" << std::endl;
            nodetmp = nodelist.top();
            std::cout << table.get_decl_code();
            std::cout << ".text\n"
                      << ".globl main\n"
                      << "main:\n"
                      << nodetmp.code;

            return 0;
            break;
        }
        case 1:{//program=>BASIC ID LBT RBT block            //main   id -> ID
            products.pop();status.pop();//block
            products.pop();status.pop();
            products.pop();status.pop();
            products.pop();status.pop();//ID
            products.pop();status.pop();//BASIC
            l = 2 ;
            break;
        }
        case 2:{//block=>LB decls stmts RB
            products.pop();status.pop();//RB
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//stmts
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//decls
            products.pop();status.pop();//LB
            nodetmp.code = nodetmp2.code + nodetmp1.code;
            nodelist.push(nodetmp);
            l = 3 ;
            break;
        }
        case 3:{//decls=>decls decl
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//decl
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//decls
            nodetmp.code = nodetmp2.code + nodetmp1.code;
            nodelist.push(nodetmp);
            l = 4 ;
            break;
        }
        case 4:{//decls=>%
            nodetmp.code = "";
            nodelist.push(nodetmp);
            l = 4 ;
            break;
        }
        case 5:{//decl=>BASIC id SEM
            products.pop();status.pop();//SEM
            products.pop();status.pop();//id
            products.pop();status.pop();//BASIC
            l = 5 ;
            break;
        }
        case 6:{//id=>volua COMMA id


            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//id
            products.pop();status.pop();//comma
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//volua
            nodetmp.code = nodetmp2.code + nodetmp1.code;
            nodelist.push(nodetmp);
            l = 6 ;
            break;
        }
        case 7:{//id=>volua
            products.pop();status.pop();//volua
            l = 6 ;
            break;
        }
        case 8:{//volua=>ID
            table.insert_s(products.top(),0,"int",1);
            nodetmp.res = products.top();
            nodetmp.code = "";
            products.pop();status.pop();//ID
            nodelist.push(nodetmp);
            l = 11 ;
            break;
        }
        case 9:{//volua=>ID LSB NUM RSB
            products.pop();status.pop();//RSB
            arraysize = atoi(products.top().c_str());
            products.pop();status.pop();//NUM
            products.pop();status.pop();//LSB
            table.insert_s(products.top(),1,"int",arraysize);//array
            arraysize = 0;
            nodetmp.res = products.top();
            nodetmp.code = "";
            products.pop();status.pop();//ID
            nodelist.push(nodetmp);
            l = 11 ;
            break;
        }
        case 10:{//volua=>ID LSB NUM RSB EQUAL LB expr RB
            products.pop();status.pop();//RB
            //nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//expr
            products.pop();status.pop();//LB
            products.pop();status.pop();//=
            products.pop();status.pop();//]
            arraysize = atoi(products.top().c_str());
            products.pop();status.pop();//num
            products.pop();status.pop();//[
            str1 = products.top();
            table.insert_s(str1,1,"int",arraysize);//array
            products.pop();status.pop();//ID
            str =  "movl $" + str1 + ",%eax\n";
            for (int i = 0; i < arraysize; i++)
            {
                nodetmp = nodelist.top();
                if (nodetmp.isNUM == true)
                {
                    nodelist.pop();
                    ss << (arraysize - i - 1)*sizeof(int);
                    ss >> str3;
                    ss.clear();
                    str += "movl  $" + nodetmp.res +"," + str3 + "(%eax)\n";
                }
                else
                {
                    std::cout << "error 10: array error,not have enough value!" << std::endl;
                    exit(1);
                }
            }
            arraysize = 0;
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 11 ;
            break;
        }
        case 11:{//volua=>ID EQUAL NUM
            nodetmp1.res = products.top();
            products.pop();status.pop();//NUM
            products.pop();status.pop();//=
            nodetmp2.res = products.top();
            products.pop();status.pop();//ID
            nodetmp.code = "movl  $" + nodetmp2.res + ",%eax\n"
                         + "movl  $" + nodetmp1.res + ",(%eax)\n";
            nodelist.push(nodetmp);
            table.insert_s(nodetmp2.res,0,"int",1);
            l = 11 ;
            break;
        }
        case 12:{//stmts=>stmts stmt
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();
            nodetmp.code = nodetmp2.code + nodetmp1.code;
            nodelist.push(nodetmp);
            l = 7 ;
            break;
        }
        case 13:{//stmts=>%
            nodetmp.code = "";
            nodelist.push(nodetmp);
            l = 7 ;
            break;
        }
        case 14:{//stmt=>stmvol SEM
            products.pop();status.pop();
            products.pop();status.pop();
            l = 8 ;
            break;
        }
        case 15:{//stmt=>IF LBT equality RBT stmt
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//stmt
            products.pop();status.pop();//)
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//equality
            products.pop();status.pop();//(
            products.pop();status.pop();//IF
            l1 = label_id ++;
            l2 = label_id ++;
            ss << l1;ss >> str1;ss.clear();
            ss << l2;ss >> str2;ss.clear();
            nodetmp.code =    nodetmp2.code
                            + LABEL_NAME_PREFIX + str1 + ":\n"
                            + "cmp $0,%eax\n"
                            + "jz " + LABEL_NAME_PREFIX + str2 + "\n"
                            + nodetmp1.code
                            + LABEL_NAME_PREFIX + str2 + ":\n";
            nodelist.push(nodetmp);
            l = 8 ;
            break;
        }
        case 16:{//stmt=>IF LBT equality RBT stmt1 ELSE stmt2
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//stmt2
            products.pop();status.pop();//ELSE
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//stmt1
            products.pop();status.pop();//)
            nodetmp3 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//equality
            products.pop();status.pop();//(
            products.pop();status.pop();//IF
            l1 = label_id ++;
            l2 = label_id ++;
            ss << l1;ss >> str1;ss.clear();
            ss << l2;ss >> str2;ss.clear();
            nodetmp.code =    nodetmp3.code
                            + "cmpl $0,%eax\n"
                            + "jz " + LABEL_NAME_PREFIX + str1 + "\n"
                            + nodetmp2.code  //if
                            + "jmp " + LABEL_NAME_PREFIX + str2 + "\n"
                            + LABEL_NAME_PREFIX + str1 + ":\n" //else
                            + nodetmp1.code
                            + LABEL_NAME_PREFIX + str2 + ":\n";
            nodelist.push(nodetmp);
            l = 8 ;
            break;
        }
        case 17:{//stmt=>FOR LBT stmvol SEM expr SEM add RBT stmt
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//stmt
            products.pop();status.pop();//)
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add
            products.pop();status.pop();//;
            nodetmp3 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//expr
            products.pop();status.pop();//;
            nodetmp4 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//stmvol
            products.pop();status.pop();//(
            products.pop();status.pop();//FOR

            int ls,lend;
            std::string strls,strlend;
            l1 = label_id ++;
            l2 = label_id ++;
            ls = label_id ++;
            l3 = label_id ++;
            lend = label_id ++;
            ss << l1;ss >> str1;ss.clear();
            ss << l2;ss >> str2;ss.clear();
            ss << ls;ss >> strls;ss.clear();
            ss << l3;ss >> str3;ss.clear();
            ss << lend;ss >> strlend;ss.clear();
            nodetmp.code =    LABEL_NAME_PREFIX + str1 + ":\n"
                            + nodetmp4.code
                            + LABEL_NAME_PREFIX + str2 + ":\n"
                            + nodetmp3.code
                            + "cmpl $0,%eax\n"
                            + "jz " + LABEL_NAME_PREFIX + strlend + "\n"
                            + LABEL_NAME_PREFIX + strls + ":\n"
                            + nodetmp1.code
                            + LABEL_NAME_PREFIX + str3 + ":\n"
                            + nodetmp2.code
                            + "jmp " + LABEL_NAME_PREFIX + str2 + "\n"
                            + LABEL_NAME_PREFIX + strlend + ":\n";

            nodelist.push(nodetmp);
            l = 8 ;
            break;
        }
        case 18:{//stmt=>WHILE LBT equality RBT stmt
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//stmt
            products.pop();status.pop();//)
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//equality
            products.pop();status.pop();//(
            products.pop();status.pop();//WHILE

            l1 = label_id ++;
            l2 = label_id ++;
            l3 = label_id ++;
            ss << l1;ss >> str1;ss.clear();
            ss << l2;ss >> str2;ss.clear();
            ss << l3;ss >> str3;ss.clear();

            str = LABEL_NAME_PREFIX + str1 + ":\n"
                + nodetmp2.code
                + "cmpl $0,%eax\n"
                + "jz " + LABEL_NAME_PREFIX + str3 + "\n"
                + LABEL_NAME_PREFIX + str2 + ":\n"
                + nodetmp1.code
                + "jmp " + LABEL_NAME_PREFIX + str1 + "\n"
                + LABEL_NAME_PREFIX + str3 + ":\n";

            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 8 ;
            break;
        }
        case 19:{//stmt=>PRINTF LBT STR paramer RBT SEM
            products.pop();status.pop();//;
            products.pop();status.pop();//)
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//paramer
            nodetmp2.res = products.top();
            products.pop();status.pop();//STR
            products.pop();status.pop();//(
            products.pop();status.pop();//PRINTF
            l1 = label_id ++;
            str = nodetmp1.code
                + "pushl $ \"" + nodetmp2.res + "\" \n"
                + "call printf\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 8 ;
            break;
        }
        case 20:{//stmt=>RETURN equality SEM
            products.pop();status.pop();//;
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//equality
            products.pop();status.pop();//RETURN
            str = nodetmp1.code
                + "leave \n"
                + "ret\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 8 ;
            break;
        }
        case 21:{//stmt=>block
            products.pop();status.pop();
            l = 8 ;
            break;
        }
        case 22:{//paramer=>paramer COMMA add
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add
            products.pop();status.pop();//,
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add
            str = nodetmp2.code
                + nodetmp1.code
                + "pushl %eax\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 20 ;
            break;
        }
        case 23:{//paramer=>%
            nodetmp.code = "";
            nodelist.push(nodetmp);
            l = 20 ;
            break;
        }
        case 24:{//stmvol=>loc EQUAL add
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add
            products.pop();status.pop();//=
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//loc
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "movl  %eax,(%esp)\n"
                + "addl  $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 12 ;
            break;
        }
        case 25:{//stmvol=>postfix
            products.pop();status.pop();
            l = 12 ;
            break;
        }
        case 26:{//loc=>ID LSB add RSB
            products.pop();status.pop();//]
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add
            products.pop();status.pop();//[
            nodetmp2.res = products.top();
            products.pop();status.pop();//ID
            if (table.find_s(nodetmp2.res) == false)
            {
                std::cout << "error 26: not define " + nodetmp2.res << std::endl;
                exit(1);
            }
            nodetmp.code =    nodetmp1.code
                            + "movl $" + nodetmp2.res + ",%ebx\n"
                            + "addl %eax,%ebx\n"
                            + "movl (%ebx),%eax\n";
            nodelist.push(nodetmp);
            l = 9 ;
            break;
        }
        case 27:{//loc=>ID
            nodetmp.res = products.top();
            if (table.find_s(nodetmp.res) == false)
            {
                std::cout << "error 27: not define " + nodetmp.res << std::endl;
                exit(1);
            }
            nodetmp.code = "movl $" + nodetmp.res + ",%eax\n";
            products.pop();status.pop();
            nodelist.push(nodetmp);
            l = 9 ;
            break;
        }
        case 28:{//expr=>expr COMMA equality
            //nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//equality
            products.pop();status.pop();//,
            //nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//expr
            //nodetmp.code = nodetmp2.code + nodetmp1.code;
            //nodelist.push(nodetmp);
            l = 10 ;
            break;
        }
        case 29:{//expr=>equality
            products.pop();status.pop();
            l = 10 ;
            break;
        }
        case 30:{//equality=>equality ASS rel
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//rel
            products.pop();status.pop();//==
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//equality
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "cmpl %eax,(%esp)\n"
                + "sete %al\n"
                + "movzbl %al,%eax\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 13 ;
            break;
        }
        case 31:{//equality=>equality UEQUAL rel
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//rel
            products.pop();status.pop();// !=
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//equality
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "cmpl %eax,(%esp)\n"
                + "setne %al\n"
                + "movzbl %al,%eax\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 13 ;
            break;
        }
        case 32:{//equality=>rel
            products.pop();status.pop();
            l = 13 ;
            break;
        }
        case 33:{//rel=>add LT add
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();
            products.pop();status.pop();//<
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "cmpl %eax,(%esp)\n"
                + "setl %al\n"
                + "movzbl %al,%eax\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 14 ;
            break;
        }
        case 34:{//rel=>add1 LE add2
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//2
            products.pop();status.pop();//<=
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//1
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "cmpl %eax,(%esp)\n"
                + "setle %al\n"
                + "movzbl %al,%eax\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 14 ;
            break;
        }
        case 35:{//rel=>add2 GT add1
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//1
            products.pop();status.pop();//>
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//2
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "cmpl %eax,(%esp)\n"
                + "setg %al\n"
                + "movzbl %al,%eax\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 14 ;
            break;
        }
        case 36:{//rel=>add2 GE add1
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add1
            products.pop();status.pop();//>=
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add2
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "cmpl %eax,(%esp)\n"
                + "setge %al\n"
                + "movzbl %al,%eax\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 14 ;
            break;
        }
        case 37:{//rel=>add
            products.pop();status.pop();
            l = 14 ;
            break;
        }
        case 38:{//add=>add PLUS mult
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//mult
            products.pop();status.pop();//+
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "addl %eax,(%esp)\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 15 ;
            break;
        }
        case 39:{//add=>add MIN mult
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//mult
            products.pop();status.pop();//-
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//add
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "subl %eax,(%esp)\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 15 ;
            break;
        }
        case 40:{//add=>mult
            products.pop();status.pop();
            l = 15 ;
            break;
        }
        case 41:{//mult=>mult MULT unary
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//unary
            products.pop();status.pop();//*
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//mult
            str = nodetmp2.code
                + "pushl %eax\n"
                + nodetmp1.code
                + "mull %eax,(%esp)\n"
                + "addl $4,%esp\n";
            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 16 ;
            break;
        }
        case 42:{//mult=>mult DEV unary
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//unary
            products.pop();status.pop();
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//mult
            str =   nodetmp2.code
                  + "pushl %eax\n"
                  + nodetmp1.code
                  + "divl %eax,(%esp)\n"
                  + "addl $4,%esp\n";

            nodetmp.code = str;
            nodelist.push(nodetmp);
            l = 16 ;
            break;
        }
        case 43:{//mult=>unary
            products.pop();status.pop();
            l = 16 ;
            break;
        }
        case 44:{//unary=>EXCLAMA unary
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//unary
            products.pop();status.pop();
            nodetmp.code = nodetmp1.code
                         + "notl %eax\n";
            nodelist.push(nodetmp);
            l = 17 ;
            break;
        }
        case 45:{//unary=>postfix
            products.pop();status.pop();
            l = 17 ;
            break;
        }
        case 46:{//postfix=>postfix INC
            products.pop();status.pop();//inc
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//postfix
            nodetmp.code = nodetmp1.code
                         + "movl %eax,%ecx \n"
                         + "incl %ecx \n"
                         + "movl %ecx,(%ebx)\n";
            nodelist.push(nodetmp);
            l = 18 ;
            break;
        }
        case 47:{//postfix=>postfix DEC
            products.pop();status.pop();
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();
            nodetmp.code = nodetmp1.code
                         + "movl %eax,%ecx \n"
                         + "decl %ecx \n"
                         + "movl %ecx,(%ebx)\n";
            nodelist.push(nodetmp);
            l = 18 ;
            break;
        }
        case 48:{//postfix=>factor
            products.pop();status.pop();
            l = 18 ;
            break;
        }
        case 49:{//factor=>LBT equality RBT
            products.pop();status.pop();
            products.pop();status.pop();
            products.pop();status.pop();
            l = 19 ;
            break;
        }
        case 50:{//factor=>loc
            products.pop();status.pop();
            l = 19 ;
            break;
        }
        case 51:{//factor=>NUM
            nodetmp.code = "";
            nodetmp.res = products.top();
            nodetmp.code = "movl $" + nodetmp.res + ",%eax\n";
            nodetmp.isNUM = true;
            nodelist.push(nodetmp);
            products.pop();status.pop();
            l = 19 ;
            break;
        }
        }
        top = status.top();
        status.push(cctab[top][l][1]);
        products.push(transdef(l));
        top = status.top();
        break;
        }
        default:
        {
            std::cout << "error!" << std::endl;
            return -1;
           // error();
        }
        }
    }
    return 0;
}//do_parse

#if 1//action table
const int parser::cctab[482][53][2]={
{{3,0},{3,0},{2,1},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{0,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,3},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,5},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,6},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,7},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{0,1},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,4},{3,0},{3,0},{3,0},{2,8},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{3,0},{3,0},{0,4},{0,4},{0,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{0,4},{0,4},{0,4},{0,4},{3,0},{0,4},{3,0},{0,4}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{2,9},{3,0},{2,10},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,11},{3,0},{3,0},{0,13},{0,13},{0,13},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,13},{0,13},{0,13},{0,13},{0,13},{3,0},{0,13},{3,0},{0,13}},
{{0,3},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,3},{3,0},{3,0},{0,3},{0,3},{0,3},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,3},{0,3},{0,3},{0,3},{0,3},{3,0},{0,3},{3,0},{0,3}},
{{3,0},{3,0},{3,0},{2,12},{3,0},{3,0},{3,0},{3,0},{2,13},{2,14},{3,0},{3,0},{2,15},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,18},{1,19},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,23},{1,24},{1,25},{3,0},{1,26},{3,0},{1,27}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,28},{3,0},{3,0},{3,0},{3,0},{2,29},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{0,21},{0,21},{3,0},{0,21},{3,0},{0,21}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,12},{0,12},{0,12},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,12},{0,12},{0,12},{0,12},{0,12},{3,0},{0,12},{3,0},{0,12}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{3,0},{3,0},{1,31},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,25},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,33},{1,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,4},{3,0},{3,0},{3,0},{2,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{3,0},{3,0},{0,4},{0,4},{0,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{0,4},{0,4},{0,4},{0,4},{3,0},{0,4},{3,0},{0,4}},
{{3,0},{0,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,37},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{2,54},{2,55},{2,56},{2,57},{2,58},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,65},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,66},{0,7},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,67},{3,0},{3,0},{3,0},{3,0},{3,0},{0,8},{0,8},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,69},{3,0},{3,0},{3,0},{3,0},{3,0},{2,70},{2,71},{2,72},{2,73},{2,74},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,75},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,76},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,77},{1,78},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{0,14},{0,14},{3,0},{0,14},{3,0},{0,14}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{2,9},{3,0},{2,79},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,11},{3,0},{3,0},{0,13},{0,13},{0,13},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,13},{0,13},{0,13},{0,13},{0,13},{3,0},{0,13},{3,0},{0,13}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,80},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{3,0},{3,0},{1,83},{1,84},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,85},{1,86},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{1,89},{1,90},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,91},{1,92},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,93},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,94},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,95},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,97},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,106},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,14},{3,0},{3,0},{2,107},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,108},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,109},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,110},{3,0},{3,0},{3,0},{3,0},{3,0},{1,111},{3,0},{1,112},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{1,113},{1,114},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,115},{1,116},{1,117},{1,118},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{1,119},{1,120},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,121},{1,122},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,123},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,124},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,125},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,5},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,5},{3,0},{3,0},{0,5},{0,5},{0,5},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,5},{0,5},{0,5},{0,5},{0,5},{3,0},{0,5},{3,0},{0,5}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,126},{3,0},{3,0},{3,0},{3,0},{2,29},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,127},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,128},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,24},{1,129},{1,130},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{1,131},{1,132},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,133},{1,134},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,135},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,136},{2,73},{2,74},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,75},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,76},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,77},{1,78},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,137},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,12},{3,0},{3,0},{3,0},{3,0},{2,13},{2,14},{3,0},{3,0},{2,15},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,18},{1,138},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,23},{1,24},{1,25},{3,0},{1,26},{3,0},{1,27}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{3,0},{2,139},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{3,0},{2,140},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,141},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,142},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{2,144},{2,145},{2,146},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{2,153},{2,145},{2,146},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{2,154},{2,145},{2,146},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{2,155},{2,145},{2,146},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,156},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,157},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,158},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,159},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,160},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{1,163},{1,164},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,165},{1,166},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,167},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,168},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,169},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,170},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,171},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,172},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,173},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,23},{0,23},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{0,20},{0,20},{3,0},{0,20},{3,0},{0,20}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{3,0},{2,174},{2,56},{2,57},{2,58},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{3,0},{2,175},{2,56},{2,57},{2,58},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,176},{2,58},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,177},{2,58},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{2,179},{2,180},{2,181},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{2,188},{2,180},{2,181},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{2,189},{2,180},{2,181},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{2,190},{2,180},{2,181},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,191},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,192},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,193},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,194},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,6},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,195},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,11},{0,11},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,196},{2,72},{2,73},{2,74},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,75},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,76},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,77},{1,78},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,197},{2,72},{2,73},{2,74},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,75},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,76},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,77},{1,78},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,198},{2,73},{2,74},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,75},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,76},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,77},{1,78},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,199},{2,73},{2,74},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,75},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,76},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,77},{1,78},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,200},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,201},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{0,2},{0,2},{3,0},{0,2},{3,0},{0,2}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{1,89},{1,90},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{1,89},{1,90},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{3,0},{3,0},{1,202},{1,203},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{1,204},{1,205},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,206},{1,207},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,208},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,209},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,210},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{3,0},{3,0},{1,202},{1,203},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{3,0},{3,0},{1,202},{1,203},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{3,0},{3,0},{1,202},{1,203},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,212},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,213},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,214},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,215},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,216},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,217},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,218},{3,0},{3,0},{3,0},{3,0},{2,219},{2,14},{3,0},{3,0},{2,220},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,221},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,222},{1,223},{1,224},{3,0},{1,225},{3,0},{1,226}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{2,228},{3,0},{3,0},{2,229},{2,230},{2,231},{2,232},{2,233},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,12},{3,0},{3,0},{3,0},{3,0},{2,240},{2,14},{3,0},{3,0},{2,15},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,18},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,23},{1,24},{1,25},{3,0},{1,26},{3,0},{1,27}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,241},{1,242},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{1,119},{1,120},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{1,119},{1,120},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{1,243},{1,244},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{1,245},{1,246},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,247},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,249},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,250},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,251},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{1,243},{1,244},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{1,243},{1,244},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{1,243},{1,244},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,252},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,9},{0,9},{3,0},{3,0},{3,0},{3,0},{1,253},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{1,131},{1,132},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{1,131},{1,132},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,254},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,255},{2,146},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,256},{2,146},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,257},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,258},{2,147},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,149},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{1,152},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,259},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,260},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{1,163},{1,164},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{1,163},{1,164},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,261},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{0,21},{0,21},{0,21},{0,21},{3,0},{0,21}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{0,15},{0,15},{1,262},{0,15},{3,0},{0,15}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,263},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,4},{3,0},{3,0},{3,0},{2,264},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{3,0},{3,0},{0,4},{0,4},{0,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{0,4},{0,4},{0,4},{0,4},{3,0},{0,4},{3,0},{0,4}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,265},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,266},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,267},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,268},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,53},{3,0},{3,0},{3,0},{2,269},{2,55},{2,56},{2,57},{2,58},{2,59},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,61},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,62},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,63},{1,64},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,270},{1,271},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,29},{0,29},{3,0},{3,0},{3,0},{3,0},{3,0},{1,272},{3,0},{1,273},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{0,37},{1,274},{1,275},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,276},{1,277},{1,278},{1,279},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{1,280},{1,281},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,282},{1,283},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,284},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,285},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,286},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{0,18},{0,18},{3,0},{0,18},{3,0},{0,18}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,287},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,288},{3,0},{3,0},{3,0},{3,0},{3,0},{2,289},{2,290},{2,291},{2,292},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,295},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,296},{1,297},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,298},{2,181},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,299},{2,181},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,178},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,301},{2,182},{2,183},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,185},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,186},{1,187},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,302},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,303},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,304},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{1,204},{1,205},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{1,204},{1,205},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,305},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,12},{3,0},{3,0},{3,0},{3,0},{2,306},{2,14},{3,0},{3,0},{2,15},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,18},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,23},{1,24},{1,25},{3,0},{1,26},{3,0},{1,27}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{0,14},{0,14},{0,14},{0,14},{3,0},{0,14}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{2,9},{3,0},{2,307},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,11},{3,0},{3,0},{0,13},{0,13},{0,13},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,13},{0,13},{0,13},{0,13},{0,13},{3,0},{0,13},{3,0},{0,13}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,308},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,14},{3,0},{3,0},{2,309},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,310},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{1,111},{3,0},{1,112},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{2,313},{2,230},{2,231},{2,232},{2,233},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,314},{3,0},{3,0},{3,0},{3,0},{3,0},{2,315},{2,316},{2,317},{2,318},{2,319},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,320},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,322},{1,323},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{3,0},{2,324},{2,231},{2,232},{2,233},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{3,0},{2,325},{2,231},{2,232},{2,233},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,326},{2,233},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,327},{2,233},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{2,329},{2,330},{2,331},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{2,338},{2,330},{2,331},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{2,339},{2,330},{2,331},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{2,340},{2,330},{2,331},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,341},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,342},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,343},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,344},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{0,19},{0,19},{3,0},{0,19},{3,0},{0,19}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,22},{0,22},{3,0},{1,345},{1,346},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{3,0},{0,40},{0,40},{1,347},{1,348},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,349},{1,350},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,351},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,288},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,352},{2,292},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,295},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,296},{1,297},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,353},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{1,245},{1,246},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{1,245},{1,246},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,354},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{2,356},{3,0},{3,0},{2,357},{2,358},{2,359},{2,360},{2,361},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{0,16},{0,16},{3,0},{0,16},{3,0},{0,16}},
{{3,0},{3,0},{3,0},{2,12},{3,0},{3,0},{3,0},{3,0},{2,13},{2,14},{3,0},{3,0},{2,15},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,18},{1,368},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,23},{1,24},{1,25},{3,0},{1,26},{3,0},{1,27}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,369},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,370},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,371},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,372},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,23},{0,23},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{0,20},{0,20},{0,20},{0,20},{3,0},{0,20}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,28},{0,28},{3,0},{3,0},{3,0},{3,0},{3,0},{1,272},{3,0},{1,273},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,373},{3,0},{3,0},{1,374},{1,375},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{1,376},{1,377},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,378},{1,379},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,380},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,314},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,381},{2,318},{2,319},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,320},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,322},{1,323},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,382},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{1,280},{1,281},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{1,280},{1,281},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{0,35},{1,383},{1,384},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{1,385},{1,386},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,387},{1,388},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,389},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,390},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,391},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{0,36},{1,383},{1,384},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{0,33},{1,383},{1,384},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{0,34},{1,383},{1,384},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,392},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,288},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,393},{2,291},{2,292},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,295},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,296},{1,297},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,288},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{2,291},{2,292},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,295},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,296},{1,297},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,288},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,395},{2,292},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,295},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,296},{1,297},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,288},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,396},{2,292},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,295},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,296},{1,297},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,397},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,398},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,399},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,29},{3,0},{3,0},{0,29},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{1,402},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{3,0},{3,0},{0,37},{3,0},{1,403},{1,404},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,405},{1,406},{1,407},{1,408},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{3,0},{0,40},{0,40},{1,409},{1,410},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,411},{1,412},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,413},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,414},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{3,0},{0,2}},
{{3,0},{3,0},{3,0},{2,218},{3,0},{3,0},{3,0},{3,0},{2,416},{2,14},{3,0},{3,0},{2,220},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,221},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,222},{1,223},{1,224},{3,0},{1,225},{3,0},{1,226}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,227},{2,417},{3,0},{3,0},{2,229},{2,230},{2,231},{2,232},{2,233},{2,234},{2,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,237},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,238},{1,239},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,218},{3,0},{3,0},{3,0},{3,0},{2,418},{2,14},{3,0},{3,0},{2,220},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,221},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,222},{1,223},{1,224},{3,0},{1,225},{3,0},{1,226}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,419},{1,242},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,12},{3,0},{3,0},{3,0},{3,0},{2,420},{2,14},{3,0},{3,0},{2,15},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,18},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,23},{1,24},{1,25},{3,0},{1,26},{3,0},{1,27}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,314},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,421},{2,317},{2,318},{2,319},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,320},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,322},{1,323},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,314},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,422},{2,317},{2,318},{2,319},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,320},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,322},{1,323},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,314},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,423},{2,318},{2,319},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,320},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,322},{1,323},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,314},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,424},{2,318},{2,319},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,320},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,322},{1,323},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,425},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,426},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,427},{2,331},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,428},{2,331},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,429},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,430},{2,332},{2,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,334},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,335},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,336},{1,337},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,431},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,432},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{3,0},{0,38},{0,38},{1,347},{1,348},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{3,0},{0,39},{0,39},{1,347},{1,348},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,433},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,10},{0,10},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{2,434},{2,358},{2,359},{2,360},{2,361},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{3,0},{2,435},{2,359},{2,360},{2,361},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{3,0},{2,436},{2,359},{2,360},{2,361},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,437},{2,361},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,438},{2,361},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{2,440},{2,441},{2,442},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{2,449},{2,441},{2,442},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{2,450},{2,441},{2,442},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{2,451},{2,441},{2,442},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,452},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,355},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,453},{2,362},{2,363},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,364},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{1,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,454},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,455},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{0,15},{0,15},{1,456},{0,15},{3,0},{0,15}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,270},{1,457},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{0,18},{0,18},{0,18},{0,18},{3,0},{0,18}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,458},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{0,17},{0,17},{3,0},{0,17},{3,0},{0,17}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{1,376},{1,377},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{1,376},{1,377},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,459},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{1,385},{1,386},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{1,385},{1,386},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,460},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,28},{3,0},{3,0},{0,28},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{1,402},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{3,0},{0,38},{0,38},{1,409},{1,410},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{3,0},{0,39},{0,39},{1,409},{1,410},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{3,0},{3,0},{0,35},{3,0},{1,461},{1,462},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{3,0},{0,40},{0,40},{1,463},{1,464},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,465},{1,466},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,36},{3,0},{3,0},{3,0},{2,467},{2,38},{2,39},{2,40},{2,41},{2,42},{2,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,46},{1,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,468},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,469},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{3,0},{3,0},{0,36},{3,0},{1,461},{1,462},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{3,0},{3,0},{0,33},{3,0},{1,461},{1,462},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{3,0},{3,0},{0,34},{3,0},{1,461},{1,462},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,470},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,218},{3,0},{3,0},{3,0},{3,0},{2,471},{2,14},{3,0},{3,0},{2,220},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,221},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,222},{1,223},{1,224},{3,0},{1,225},{3,0},{1,226}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,314},{3,0},{3,0},{3,0},{3,0},{3,0},{2,472},{2,316},{2,317},{2,318},{2,319},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,320},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,322},{1,323},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{0,19},{0,19},{0,19},{0,19},{3,0},{0,19}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,473},{2,442},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,474},{2,442},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,475},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,439},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,476},{2,443},{2,444},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,446},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,447},{1,448},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,477},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,81},{3,0},{1,82},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,96},{3,0},{3,0},{3,0},{3,0},{3,0},{2,478},{2,98},{2,99},{2,100},{2,101},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,103},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,104},{1,105},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{0,16},{0,16},{0,16},{0,16},{3,0},{0,16}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,479},{3,0},{3,0},{1,374},{1,375},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{3,0},{0,38},{0,38},{1,463},{1,464},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{3,0},{0,39},{0,39},{1,463},{1,464},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,480},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,161},{1,162},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,218},{3,0},{3,0},{3,0},{3,0},{2,481},{2,14},{3,0},{3,0},{2,220},{3,0},{3,0},{3,0},{3,0},{3,0},{2,16},{2,17},{3,0},{3,0},{3,0},{3,0},{1,221},{3,0},{1,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{1,22},{1,222},{1,223},{1,224},{3,0},{1,225},{3,0},{1,226}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{0,17},{0,17},{0,17},{0,17},{3,0},{0,17}}
};
#endif //action table

