#pragma once

// std::exception
#include <exception>

// std::string
#include <string>

// Location macro
#define S1(x) #x
#define S2(x) S1(x)
#define __LOC__ std::string("File \"" __FILE__ "\", line " S2(__LINE__))

namespace Exception {
    // exception -> unit 
    void print_exception(std::exception const & e);

    // exception -> std::string 
    std::string exception_to_string(std::exception const & e);
}
