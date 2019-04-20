#include <iostream>
#include <experimental/iterator>
#include "print_iter.h"

template<typename T>
void printIterable(const T& ctn, const char *name) {
    std::cout << " /// " << name << ": [";
    std::copy(ctn.cbegin(), ctn.cend(), std::experimental::make_ostream_joiner(std::cout, "; "));
    std::cout << "]" << std::endl;
}


