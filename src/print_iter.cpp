#include <iostream>
#include <experimental/iterator>
#include <vector>
#include <deque>
#include "print_iter.h"

template<>
void printIterable(const std::vector<size_t>& ctn, const char *name) {
    std::cout << " /// " << name << ": [";
    std::copy(ctn.cbegin(), ctn.cend(), std::experimental::make_ostream_joiner(std::cout, "; "));
    std::cout << "]" << std::endl;
}

template<>
void printIterable(const std::deque<size_t>& ctn, const char *name) {
    std::cout << " /// " << name << ": [";
    std::copy(ctn.cbegin(), ctn.cend(), std::experimental::make_ostream_joiner(std::cout, "; "));
    std::cout << "]" << std::endl;
}


