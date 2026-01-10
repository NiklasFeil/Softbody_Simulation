#include "Global.hpp"
#include <iostream>

namespace Global {
        
    void print_error_callback(int num, const char* str) {
    std::cerr << "Error " << num << ": " << str << std::endl;
    }

}