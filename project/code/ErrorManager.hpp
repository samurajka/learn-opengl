#ifndef ERROR_MANAGER_HPP
#define ERROR_MANAGER_HPP

#include "string"
#include <iostream>

namespace error{
    int LOG_LEVEL = 3;
    /*
        1 = application breaking error
        2 = object breaking error
        3 = error that doesnt completely break object but makes it behave incorectly
        4 = not critical error
        5 = debug messages
    */

    void PrintMessage(int messagePriority, std::string text){
        if (LOG_LEVEL >= messagePriority){
            std::cout << "ERROR MANAGER, priority "<< messagePriority << ", " << text << std::endl;
        }
    }
};

#endif