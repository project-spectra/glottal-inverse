#ifndef PRINT_ITER_H
#define PRINT_ITER_H


#include <iostream>
#include <vector>
#include <deque>

std::ostream& operator <<(std::ostream& out, const std::pair<int, bool> c);

template<typename T>
std::ostream& operator <<(std::ostream& out, std::vector<T>& ctn);

template<typename T>
std::ostream& operator <<(std::ostream& out, std::deque<T>& ctn);


#endif // PRINT_ITER_H
