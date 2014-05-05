#include "signtable.h"
#include <string>

int main()
{
    SymbolTable* Sign;
    Sign->begin();
    std::cout << "0" << std::endl;
    std::string a[2] = {"abc","c"};
    std::cout << "0.1" << std::endl;
    Sign->insert_s((char *)a[0].c_str(),1,1);
    std::cout << "1" << std::endl;
    Sign->insert_s((char *)a[1].c_str(),1,2);
    std::cout << "2" << std::endl;
    Sign->show_st();
   // Sign -> end();
}
