#ifndef _SIGNTABLE_H_
#define _SIGNTABLE_H_

#define BUCKETS 109
#include <stdlib.h>
#include <iostream>
#include <stack>
#include <sstream>


struct STable
{
    std::string name;//名字
    int sign;//符号种类 val -> 0 array -> 1
    std::string type;//类型 int
    int addr;//地址
    int s;
    struct STable *next;//扩展属性地址
};

class SymbolTable
{
    public:
        SymbolTable();
        //SymbolTable(int set);
        ~SymbolTable(){};
        void begin();
       // void end();
        int comp_add(std::string name);//计算符号在符号表的位置
        void insert_s(std::string name,int sign,std::string tp,int s);//在符号表中插入一个符号
       // void dele_s();//删除符号表中的一个符号
        int get_offset();
        bool find_s(std::string name);//是否存在该符号
        STable* search_s(std::string name);//找到了则返回符号信息
        std::stack<STable*> search_type(int sign);
        void show_st();//展示符号表
        std::string get_decl_code()
        {
                std::stringstream code;
                code<<".data\n";
                    for(int i = 0; i < BUCKETS; i++)
                    {
                        STable* tmp = ST[i];
                        while (tmp != NULL)
                        {
                            //if(tmp -> ){
                                    code << tmp -> name << ":\n"
                                         << "."<<tmp -> type << " " ;

                                    for(int i = 0 ; i < tmp -> s - 1;i++){
                                        code << "0,";
                                    }
                                    code << "0" << std::endl;
                            //}
                            tmp = tmp -> next;
                        }
                    }
                return code.str();
        }
    private:
        STable *ST[BUCKETS];
        int offset;
};

SymbolTable::SymbolTable()
{
    offset = 0;
    for (int i = 0; i < BUCKETS; i++)
    {
        ST[i] = NULL;
    }
}

/*SymbolTable::~SymbolTable()
{
    for(int i = 0; i < BUCKETS; i++)
    {
        STable *tmp = ST[i];
        while(tmp != NULL)
        {
            if (tmp -> next == NULL)
            {
                delete tmp;
                tmp = ST[i];
            }
        }
    }
}*/

/*void SymbolTable::end()
{
    std::cout << "delete" << std::endl;
    for(int i = 0; i < BUCKETS; i++)
    {
        std::cout << i << std::endl;
        STable *tmp = ST[i];
        while(tmp != NULL)
        {
            if (tmp -> next == NULL)
            {
                //std::cout << tmp -> name << std::endl;
                delete tmp;
                //tmp = NULL;
                tmp = ST[i];
            }
            tmp = tmp -> next;
        }
    }
}*/

int SymbolTable::get_offset()
{
    return offset;
}

void SymbolTable::insert_s(std::string name,int sign,std::string tp,int s)
{
    if (find_s(name)==true)
    {
        std::cout << "error:rename" << std::endl;
        return;
    }
    int place = comp_add(name);
    if (ST[place] == NULL)
    {
        ST[place] = new STable;
        ST[place] -> name = name;
        ST[place] -> sign = sign;
        ST[place] -> type = tp;
        ST[place] -> addr = offset;
        offset   = offset + (sizeof(int)*s);
        ST[place] -> s    = s;
        ST[place] -> next = NULL;
    }
    else
    {
        STable *tmp = ST[place];
        while ((tmp -> next) != NULL)
        {
            tmp = tmp -> next;
        }
        STable *p = new STable;
        p -> name = name;
        p -> sign = sign;
        p -> type = tp;
        p -> addr = offset;
        offset    = offset + sizeof(int)*s;
        p -> s    = s;
        p -> next = NULL;
        tmp -> next = p;
    }
}

int SymbolTable::comp_add(std::string name)
{
    //int snum = atoi((const char*)name);
    int snum = 0, i = 0;
    while(name[i] != '\0')
    {
        snum = snum + (name[i] - 0);
        i++;
    }
    return snum%BUCKETS;
}

bool SymbolTable::find_s(std::string name)
{
    int place = comp_add(name);
    STable *tmp = ST[place];
    while (tmp != NULL)
    {
        if ((tmp -> name) == name)
        {
            return true;
        }
        tmp = tmp -> next;
    }
    return false;
}

STable* SymbolTable::search_s(std::string name)
{
    int place = comp_add(name);
    STable *tmp = ST[place];
    while (tmp != NULL)
    {
        if ((tmp -> name) == name)
        {
            return tmp;
        }
        tmp = tmp -> next;
    }
    std::cout << "error:not search it!" << std::endl;
    return NULL;
}

void SymbolTable::show_st()
{
    std::cout << "i: " << " name\t" << "sign\t" << "type\t" << "add\t" << "size\t" << std::endl;
    for(int i = 0; i < BUCKETS; i++)
    {
        STable* tmp = ST[i];
        while (tmp != NULL)
        {
            std::cout << i << ": " << tmp -> name << "     \t" << tmp -> sign << "\t" << tmp -> type << "\t" << tmp -> addr << "\t"  << tmp -> s << std::endl;
            tmp = tmp -> next;
        }
    }
}

/*std::stack<STable*> SymbolTable::search_type(int sign)
{
    std::stack<STable*> t;
    for (int i = 0; i < BUCKETS; i++)
    {
        STable* tmp = ST[i];
        while (tmp != NULL)
        {
            if (tmp -> sign == sign)
            {
                t.push(tmp);
            }
            tmp = tmp -> next;
        }
    }
}*/
#endif
