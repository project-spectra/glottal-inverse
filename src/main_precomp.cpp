#ifdef PRECOMP

#include <iostream>
#include <cstdlib>
#include <omp.h>

#include "amgif.h"
#include "persist_c.h"


int main() {
    std::cout << "   -=- +   Computing operator C   + -=-" << std::endl;

    int maxThreads;
    
    maxThreads = omp_get_max_threads();
    omp_set_num_threads(maxThreads);

    computeC();

    return EXIT_SUCCESS;
}

#endif
