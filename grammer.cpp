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
        //std::cout << "case: " << cctab[top][sym][1] << std::endl;
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
            nodetmp1 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//block
            products.pop();status.pop();
            products.pop();status.pop();
            nodetmp2 = nodelist.top();nodelist.pop();
            products.pop();status.pop();//id
            products.pop();status.pop();//BASIC
            nodetmp.code = nodetmp2.code + nodetmp1.code;
            nodelist.push(nodetmp);
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
const int parser::cctab[495][53][2]={
{{3,0},{3,0},{2,1},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{0,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,3},{3,0},{3,0},{3,0},{3,0},{2,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,5},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,6},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,7},{3,0},{1,7},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,8},{3,0},{3,0},{3,0},{0,8},{3,0},{0,8},{3,0},{3,0},{3,0},{3,0},{3,0},{1,9},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,10},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,11},{3,0},{3,0},{3,0},{3,0},{2,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,5},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,12},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,13},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,14},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,15},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,6},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,16},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,11},{3,0},{0,11},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{0,1},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,4},{3,0},{3,0},{3,0},{2,17},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{3,0},{3,0},{0,4},{0,4},{0,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{0,4},{0,4},{0,4},{0,4},{3,0},{0,4},{3,0},{0,4}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,9},{3,0},{0,9},{3,0},{3,0},{3,0},{3,0},{3,0},{1,18},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{2,19},{3,0},{2,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{3,0},{3,0},{0,13},{0,13},{0,13},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,13},{0,13},{0,13},{0,13},{0,13},{3,0},{0,13},{3,0},{0,13}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,22},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,3},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,3},{3,0},{3,0},{0,3},{0,3},{0,3},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,3},{0,3},{0,3},{0,3},{0,3},{3,0},{0,3},{3,0},{0,3}},
{{3,0},{3,0},{3,0},{2,23},{3,0},{3,0},{3,0},{3,0},{2,24},{2,25},{3,0},{3,0},{2,26},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,29},{1,30},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,34},{1,35},{1,36},{3,0},{1,37},{3,0},{1,38}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,39},{3,0},{3,0},{3,0},{3,0},{2,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{2,43},{3,0},{3,0},{2,44},{2,45},{2,46},{2,47},{2,48},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{0,21},{0,21},{3,0},{0,21},{3,0},{0,21}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,12},{0,12},{0,12},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,12},{0,12},{0,12},{0,12},{0,12},{3,0},{0,12},{3,0},{0,12}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{3,0},{3,0},{1,55},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,56},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,25},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,57},{1,58},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,4},{3,0},{3,0},{3,0},{2,59},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{3,0},{3,0},{0,4},{0,4},{0,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{0,4},{0,4},{0,4},{0,4},{3,0},{0,4},{3,0},{0,4}},
{{3,0},{0,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,61},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,72},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,73},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,74},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,75},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,76},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{2,78},{2,79},{2,80},{2,81},{2,82},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,89},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,90},{0,7},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,91},{3,0},{3,0},{3,0},{3,0},{3,0},{0,8},{0,8},{3,0},{3,0},{3,0},{3,0},{1,92},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,93},{3,0},{3,0},{1,94},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,29},{3,0},{3,0},{0,29},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,95},{3,0},{1,96},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{3,0},{3,0},{0,37},{3,0},{1,97},{1,98},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,99},{1,100},{1,101},{1,102},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{3,0},{0,40},{0,40},{1,103},{1,104},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,105},{1,106},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,107},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,108},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,109},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,110},{3,0},{3,0},{3,0},{3,0},{3,0},{2,111},{2,112},{2,113},{2,114},{2,115},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,116},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,117},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,118},{1,119},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{0,14},{0,14},{3,0},{0,14},{3,0},{0,14}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{2,19},{3,0},{2,120},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{3,0},{3,0},{0,13},{0,13},{0,13},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,13},{0,13},{0,13},{0,13},{0,13},{3,0},{0,13},{3,0},{0,13}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,121},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{3,0},{3,0},{1,124},{1,125},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,126},{1,127},{1,128},{1,129},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{1,130},{1,131},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,132},{1,133},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,134},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,135},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,136},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,138},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,147},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,25},{3,0},{3,0},{2,148},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,149},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,150},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,151},{3,0},{3,0},{3,0},{3,0},{3,0},{1,152},{3,0},{1,153},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{1,154},{1,155},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,156},{1,157},{1,158},{1,159},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{1,160},{1,161},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,162},{1,163},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,164},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,165},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,166},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,5},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,5},{3,0},{3,0},{0,5},{0,5},{0,5},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,5},{0,5},{0,5},{0,5},{0,5},{3,0},{0,5},{3,0},{0,5}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,167},{3,0},{3,0},{3,0},{3,0},{2,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,168},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,169},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,10},{3,0},{0,10},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{2,170},{2,45},{2,46},{2,47},{2,48},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{3,0},{2,171},{2,46},{2,47},{2,48},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{3,0},{2,172},{2,46},{2,47},{2,48},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,173},{2,48},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,174},{2,48},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{2,176},{2,177},{2,178},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{2,185},{2,177},{2,178},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{2,186},{2,177},{2,178},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{2,187},{2,177},{2,178},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,188},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,189},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,190},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,191},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,24},{1,192},{1,193},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{1,194},{1,195},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,196},{1,197},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,198},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,110},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,199},{2,114},{2,115},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,116},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,117},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,118},{1,119},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,200},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,23},{3,0},{3,0},{3,0},{3,0},{2,24},{2,25},{3,0},{3,0},{2,26},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,29},{1,201},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,34},{1,35},{1,36},{3,0},{1,37},{3,0},{1,38}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{3,0},{2,202},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{3,0},{2,203},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,204},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,205},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{2,207},{2,208},{2,209},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{2,216},{2,208},{2,209},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{2,217},{2,208},{2,209},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{2,218},{2,208},{2,209},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,219},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,220},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,221},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,222},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,223},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{1,226},{1,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,228},{1,229},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,230},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,231},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,232},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,233},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,234},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,235},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,236},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,23},{0,23},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{0,20},{0,20},{3,0},{0,20},{3,0},{0,20}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{3,0},{2,237},{2,80},{2,81},{2,82},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{3,0},{2,238},{2,80},{2,81},{2,82},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,239},{2,82},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,240},{2,82},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{2,242},{2,243},{2,244},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{2,251},{2,243},{2,244},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{2,252},{2,243},{2,244},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{2,253},{2,243},{2,244},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,254},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,255},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,256},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,257},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,6},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,258},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,11},{0,11},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,28},{3,0},{3,0},{0,28},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,95},{3,0},{1,96},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{3,0},{0,38},{0,38},{1,103},{1,104},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{3,0},{0,39},{0,39},{1,103},{1,104},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{3,0},{3,0},{0,35},{3,0},{1,259},{1,260},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{3,0},{0,40},{0,40},{1,261},{1,262},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,263},{1,264},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,265},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,266},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,267},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{3,0},{3,0},{0,36},{3,0},{1,259},{1,260},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{3,0},{3,0},{0,33},{3,0},{1,259},{1,260},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{3,0},{3,0},{0,34},{3,0},{1,259},{1,260},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,268},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,110},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,269},{2,113},{2,114},{2,115},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,116},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,117},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,118},{1,119},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,110},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,270},{2,113},{2,114},{2,115},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,116},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,117},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,118},{1,119},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,110},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,271},{2,114},{2,115},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,116},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,117},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,118},{1,119},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,110},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,272},{2,114},{2,115},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,116},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,117},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,118},{1,119},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,273},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,274},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{0,2},{0,2},{3,0},{0,2},{3,0},{0,2}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{1,130},{1,131},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{1,130},{1,131},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{3,0},{3,0},{1,275},{1,276},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{1,277},{1,278},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,279},{1,280},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,281},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,282},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,283},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{3,0},{3,0},{1,275},{1,276},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{3,0},{3,0},{1,275},{1,276},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{3,0},{3,0},{1,275},{1,276},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,284},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,285},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,286},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,287},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,288},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,289},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,290},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,291},{3,0},{3,0},{3,0},{3,0},{2,292},{2,25},{3,0},{3,0},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,295},{1,296},{1,297},{3,0},{1,298},{3,0},{1,299}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{2,301},{3,0},{3,0},{2,302},{2,303},{2,304},{2,305},{2,306},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,23},{3,0},{3,0},{3,0},{3,0},{2,313},{2,25},{3,0},{3,0},{2,26},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,29},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,34},{1,35},{1,36},{3,0},{1,37},{3,0},{1,38}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,314},{1,315},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{1,160},{1,161},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{1,160},{1,161},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{1,316},{1,317},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{1,318},{1,319},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,320},{1,321},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,322},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,323},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,324},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{1,316},{1,317},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{1,316},{1,317},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{1,316},{1,317},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,325},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,9},{0,9},{3,0},{3,0},{3,0},{3,0},{1,326},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,327},{2,178},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,328},{2,178},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,329},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,175},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,330},{2,179},{2,180},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,181},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,182},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,183},{1,184},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,331},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,332},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{1,194},{1,195},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{1,194},{1,195},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,333},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,334},{2,209},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,335},{2,209},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,336},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,206},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,337},{2,210},{2,211},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,212},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,213},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,214},{1,215},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,338},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,339},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{1,226},{1,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{1,226},{1,227},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,340},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,21},{0,21},{0,21},{0,21},{0,21},{0,21},{0,21},{3,0},{0,21}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{0,15},{0,15},{1,341},{0,15},{3,0},{0,15}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,342},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{0,4},{3,0},{3,0},{3,0},{2,343},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{3,0},{3,0},{0,4},{0,4},{0,4},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,4},{0,4},{0,4},{0,4},{0,4},{3,0},{0,4},{3,0},{0,4}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,344},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,345},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,346},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,347},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,77},{3,0},{3,0},{3,0},{2,348},{2,79},{2,80},{2,81},{2,82},{2,83},{2,84},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,85},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,86},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,87},{1,88},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,349},{1,350},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,29},{0,29},{3,0},{3,0},{3,0},{3,0},{3,0},{1,351},{3,0},{1,352},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{0,32},{3,0},{0,32},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,37},{0,37},{1,353},{1,354},{3,0},{3,0},{3,0},{0,37},{3,0},{0,37},{3,0},{3,0},{1,355},{1,356},{1,357},{1,358},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{1,359},{1,360},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,361},{1,362},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,363},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,364},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,365},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{0,18},{0,18},{3,0},{0,18},{3,0},{0,18}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,366},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,367},{3,0},{3,0},{3,0},{3,0},{3,0},{2,368},{2,369},{2,370},{2,371},{2,372},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,373},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,374},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,375},{1,376},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,377},{2,244},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,378},{2,244},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,379},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,241},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,380},{2,245},{2,246},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,247},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,248},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,249},{1,250},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,381},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,382},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,383},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{3,0},{0,38},{0,38},{1,261},{1,262},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{3,0},{0,39},{0,39},{1,261},{1,262},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,384},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{1,277},{1,278},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{1,277},{1,278},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,385},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,23},{3,0},{3,0},{3,0},{3,0},{2,386},{2,25},{3,0},{3,0},{2,26},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,29},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,34},{1,35},{1,36},{3,0},{1,37},{3,0},{1,38}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,14},{0,14},{0,14},{0,14},{0,14},{0,14},{0,14},{3,0},{0,14}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{2,19},{3,0},{2,387},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,21},{3,0},{3,0},{0,13},{0,13},{0,13},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,13},{0,13},{0,13},{0,13},{0,13},{3,0},{0,13},{3,0},{0,13}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,388},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,25},{3,0},{3,0},{2,389},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,390},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,391},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,392},{3,0},{3,0},{3,0},{3,0},{3,0},{1,152},{3,0},{1,153},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{2,393},{2,303},{2,304},{2,305},{2,306},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{3,0},{3,0},{3,0},{3,0},{3,0},{2,395},{2,396},{2,397},{2,398},{2,399},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,402},{1,403},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{3,0},{2,404},{2,304},{2,305},{2,306},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{3,0},{2,405},{2,304},{2,305},{2,306},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,406},{2,306},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,407},{2,306},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{2,409},{2,410},{2,411},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{2,418},{2,410},{2,411},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{2,419},{2,410},{2,411},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{2,420},{2,410},{2,411},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,421},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,422},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,423},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,424},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{0,19},{0,19},{3,0},{0,19},{3,0},{0,19}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,22},{0,22},{3,0},{1,425},{1,426},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{3,0},{0,40},{0,40},{1,427},{1,428},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,429},{1,430},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,431},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,432},{2,371},{2,372},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,373},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,374},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,375},{1,376},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,433},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{1,318},{1,319},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{1,318},{1,319},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,434},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,42},{2,435},{3,0},{3,0},{2,44},{2,45},{2,46},{2,47},{2,48},{2,49},{2,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,52},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,53},{1,54},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{0,16},{0,16},{3,0},{0,16},{3,0},{0,16}},
{{3,0},{3,0},{3,0},{2,23},{3,0},{3,0},{3,0},{3,0},{2,24},{2,25},{3,0},{3,0},{2,26},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,29},{1,436},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,34},{1,35},{1,36},{3,0},{1,37},{3,0},{1,38}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,437},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,438},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,439},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,440},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,23},{0,23},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,20},{0,20},{0,20},{0,20},{0,20},{0,20},{0,20},{3,0},{0,20}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,28},{0,28},{3,0},{3,0},{3,0},{3,0},{3,0},{1,351},{3,0},{1,352},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,441},{3,0},{3,0},{1,442},{1,443},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{3,0},{3,0},{0,40},{0,40},{1,444},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{3,0},{3,0},{3,0},{3,0},{1,446},{1,447},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,448},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,449},{2,398},{2,399},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,402},{1,403},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,450},{3,0},{3,0},{3,0},{3,0},{0,27},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{0,31},{3,0},{0,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{0,30},{3,0},{0,30},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{1,359},{1,360},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{1,359},{1,360},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,50},{0,50},{0,50},{0,50},{0,50},{0,50},{3,0},{0,50},{3,0},{0,50},{0,50},{0,50},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,35},{0,35},{1,451},{1,452},{3,0},{3,0},{3,0},{0,35},{3,0},{0,35},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,40},{0,40},{0,40},{0,40},{1,453},{1,454},{3,0},{0,40},{3,0},{0,40},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,43},{0,43},{0,43},{0,43},{0,43},{0,43},{3,0},{0,43},{3,0},{0,43},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,45},{0,45},{0,45},{0,45},{0,45},{0,45},{3,0},{0,45},{3,0},{0,45},{1,455},{1,456},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,48},{0,48},{0,48},{0,48},{0,48},{0,48},{3,0},{0,48},{3,0},{0,48},{0,48},{0,48},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,60},{3,0},{3,0},{3,0},{2,457},{2,62},{2,63},{2,64},{2,65},{2,66},{2,67},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,68},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,69},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,70},{1,71},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,458},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,459},{3,0},{3,0},{3,0},{3,0},{3,0},{0,27},{0,27},{0,27},{0,27},{0,27},{0,27},{3,0},{0,27},{3,0},{0,27},{0,27},{0,27},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,51},{0,51},{0,51},{0,51},{0,51},{0,51},{3,0},{0,51},{3,0},{0,51},{0,51},{0,51},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,36},{0,36},{1,451},{1,452},{3,0},{3,0},{3,0},{0,36},{3,0},{0,36},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,33},{0,33},{1,451},{1,452},{3,0},{3,0},{3,0},{0,33},{3,0},{0,33},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,34},{0,34},{1,451},{1,452},{3,0},{3,0},{3,0},{0,34},{3,0},{0,34},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,460},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,461},{2,370},{2,371},{2,372},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,373},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,374},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,375},{1,376},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,462},{2,370},{2,371},{2,372},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,373},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,374},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,375},{1,376},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,463},{2,371},{2,372},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,373},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,374},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,375},{1,376},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,367},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,464},{2,371},{2,372},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,373},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,374},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,375},{1,376},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,465},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,466},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,467},{3,0},{3,0},{1,94},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{3,0},{0,2}},
{{3,0},{3,0},{3,0},{2,291},{3,0},{3,0},{3,0},{3,0},{2,468},{2,25},{3,0},{3,0},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,295},{1,296},{1,297},{3,0},{1,298},{3,0},{1,299}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,300},{2,469},{3,0},{3,0},{2,302},{2,303},{2,304},{2,305},{2,306},{2,307},{2,308},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,309},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,310},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,311},{1,312},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,291},{3,0},{3,0},{3,0},{3,0},{2,470},{2,25},{3,0},{3,0},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,295},{1,296},{1,297},{3,0},{1,298},{3,0},{1,299}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,471},{1,315},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,23},{3,0},{3,0},{3,0},{3,0},{2,472},{2,25},{3,0},{3,0},{2,26},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,29},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,34},{1,35},{1,36},{3,0},{1,37},{3,0},{1,38}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,473},{2,397},{2,398},{2,399},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,402},{1,403},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,474},{2,397},{2,398},{2,399},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,402},{1,403},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,475},{2,398},{2,399},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,402},{1,403},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,476},{2,398},{2,399},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,402},{1,403},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,477},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,478},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,479},{2,411},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,480},{2,411},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,481},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,408},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,482},{2,412},{2,413},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,414},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,415},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,416},{1,417},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,46},{0,46},{0,46},{0,46},{0,46},{0,46},{3,0},{0,46},{3,0},{0,46},{0,46},{0,46},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,47},{0,47},{0,47},{0,47},{0,47},{0,47},{3,0},{0,47},{3,0},{0,47},{0,47},{0,47},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,483},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,122},{3,0},{1,123},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,44},{0,44},{0,44},{0,44},{0,44},{0,44},{3,0},{0,44},{3,0},{0,44},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,137},{3,0},{3,0},{3,0},{3,0},{3,0},{2,484},{2,139},{2,140},{2,141},{2,142},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,143},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,144},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,145},{1,146},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{3,0},{0,38},{0,38},{1,427},{1,428},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{3,0},{0,39},{0,39},{1,427},{1,428},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,485},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,10},{0,10},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,15},{0,15},{0,15},{0,15},{0,15},{1,486},{0,15},{3,0},{0,15}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,349},{1,487},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,18},{0,18},{0,18},{0,18},{0,18},{0,18},{0,18},{3,0},{0,18}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,488},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{0,17},{0,17},{3,0},{0,17},{3,0},{0,17}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{3,0},{3,0},{0,38},{0,38},{1,444},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{3,0},{3,0},{0,39},{0,39},{1,444},{1,445},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,489},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,38},{0,38},{0,38},{0,38},{1,453},{1,454},{3,0},{0,38},{3,0},{0,38},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,39},{0,39},{0,39},{0,39},{1,453},{1,454},{3,0},{0,39},{3,0},{0,39},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,41},{0,41},{0,41},{0,41},{0,41},{0,41},{3,0},{0,41},{3,0},{0,41},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,42},{0,42},{0,42},{0,42},{0,42},{0,42},{3,0},{0,42},{3,0},{0,42},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,49},{0,49},{0,49},{0,49},{0,49},{0,49},{3,0},{0,49},{3,0},{0,49},{0,49},{0,49},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,490},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,224},{1,225},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,291},{3,0},{3,0},{3,0},{3,0},{2,491},{2,25},{3,0},{3,0},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,295},{1,296},{1,297},{3,0},{1,298},{3,0},{1,299}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{2,394},{3,0},{3,0},{3,0},{3,0},{3,0},{2,492},{2,396},{2,397},{2,398},{2,399},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,400},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,401},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,402},{1,403},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,19},{0,19},{0,19},{0,19},{0,19},{0,19},{0,19},{3,0},{0,19}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,26},{0,26},{0,26},{0,26},{0,26},{0,26},{3,0},{0,26},{3,0},{0,26},{0,26},{0,26},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,16},{0,16},{0,16},{0,16},{0,16},{0,16},{0,16},{3,0},{0,16}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,493},{3,0},{3,0},{1,442},{1,443},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0}},
{{3,0},{3,0},{3,0},{2,291},{3,0},{3,0},{3,0},{3,0},{2,494},{2,25},{3,0},{3,0},{2,293},{3,0},{3,0},{3,0},{3,0},{3,0},{2,27},{2,28},{3,0},{3,0},{3,0},{3,0},{1,294},{3,0},{1,31},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{1,32},{1,33},{1,295},{1,296},{1,297},{3,0},{1,298},{3,0},{1,299}},
{{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{3,0},{0,17},{0,17},{0,17},{0,17},{0,17},{0,17},{0,17},{3,0},{0,17}}
};
#endif //action table

