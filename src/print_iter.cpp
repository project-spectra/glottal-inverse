#include <vector>
#include <iterator>
#include "print_iter.h"


#define PRINT_ITER_IMPL(out, ctn, T)  out << "["; std::copy((ctn).cbegin(), (ctn).cend(), std::ostream_iterator<T>(out, "; ")); return out << "]";

template<> std::ostream& operator <<(std::ostream& out, std::vector<int>& ctn) {
    PRINT_ITER_IMPL(out, ctn, int)
}

template<> std::ostream& operator <<(std::ostream& out, std::vector<double>& ctn) {
    PRINT_ITER_IMPL(out, ctn, double)
}

template<> std::ostream& operator <<(std::ostream& out, std::deque<int>& ctn) {
    PRINT_ITER_IMPL(out, ctn, int)
}
