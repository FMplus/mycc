#ifndef _SIGNTABLE_H_
#define _SIGNTABLE_H_

#define BUCKETS 109
#include <stdlib.h>
#include <iostream>

struct STable
{
    char* name;//名字
    int sign;//符号种类
    int type;//类型
    int add;//地址
    int size;
    struct STable *next;//扩展属性地址
};

class SymbolTable
{
    public:
        SymbolTable();
        //SymbolTable(int set);
        ~SymbolTable();
        void begin();
       // void end();
        int comp_add(char* name);//计算符号在符号表的位置
        void insert_s(char* name,int sign,int type,int size);//在符号表中插入一个符号
       // void dele_s();//删除符号表中的一个符号
        int get_offset();
        bool find_s(char *name);//是否存在该符号
        STable* search_s(char *name);//找到了则返回符号信息
        void show_st();//展示符号表
    private:
        STable *ST[BUCKETS];
        int offset;
};

SymbolTable::SymbolTable()
{
    offset = 0;
    for (int i = 0; i < BUCKETS; i++)
    {
        std::cout << i << " " ;
        ST[i] = NULL;
    }
    std::cout << std::endl;
}

SymbolTable::~SymbolTable()
{
    //std::cout << "delete" << std::endl;
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
}

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

void SymbolTable::begin()
{
    offset = 0;
    for (int i = 0; i < BUCKETS; i++)
    {
        std::cout << i << " " ;
        ST[i] = NULL;
    }
    std::cout << std::endl;
}

/*SymbolTable::SymbolTable(int set)
{
    offset = set;
    for (int i = 0; i < BUCKETS; i++)
    {
        std::cout << i << " " ;
        ST[i] = NULL;
    }
    std::cout << std::endl;
}*/

int SymbolTable::get_offset()
{
    return offset;
}

void SymbolTable::insert_s(char* name,int sign,int type,int size)
{
    if (find_s(name)==true)
    {
        std::cout << "error:rename" << std::endl;
        return;
    }
    std::cout << name << std::endl;
    int place;
    //std::cout << "comp_add: " << comp_add(name) << std::endl;
    place = comp_add(name);

    if (ST[place] == NULL)
    {
        ST[place] = new STable;
        ST[place] -> name = name;
        ST[place] -> sign = sign;
        ST[place] -> type = type;
        ST[place] -> add  = offset;
        offset = offset + sizeof(int)*size;
        ST[place] -> size = size;
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
        p -> type = type;
        p -> add  = offset;
        offset = offset + sizeof(int)*size;
        p -> size = size;
        p -> next = NULL;
        tmp -> next = p;
    }
}

int SymbolTable::comp_add(char* name)
{
    //int snum = atoi((const char*)name);
    int snum = 0,i=0;
    while(name[i] != '\0')
    {
        snum = snum + (name[i] - 0);
        i++;
    }
    //std::cout << "snum: " << name[0] << std::endl;
    int res = snum%BUCKETS ;
    //std::cout << "res: " << res << std::endl;
    return res;
}

bool SymbolTable::find_s(char *name)
{
    int place = comp_add(name);
    //std::cout << "place: "  << place << std::endl;
    STable *tmp = ST[place];
    //std::cout << "NULL" << std::endl;
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

STable* SymbolTable::search_s(char* name)
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
    return NULL;
}

void SymbolTable::show_st()
{
    std::cout << "i: " << " name   |" << "sign   |" << "type   |" << "add   |"<< std::endl;
    for(int i = 0; i < BUCKETS; i++)
    {
        STable* tmp = ST[i];
        while (tmp != NULL)
        {
            std::cout << i << ": " << tmp -> name << " " << tmp -> sign << " " << tmp -> type << " " << tmp -> add << std::endl;
            tmp = tmp -> next;
        }
    }
}
#endif
