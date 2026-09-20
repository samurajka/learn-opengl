#ifndef ERROR_MANAGER_HPP
#define ERROR_MANAGER_HPP

#include "string"
#include <iostream>

namespace error{
    int LOG_LEVEL = 3;

    void PrintMessage(int messagePriority, std::string text){
        if (LOG_LEVEL >= messagePriority){
            std::cout << "ERROR: " << text << std::endl;
        }
    }
};

#endif