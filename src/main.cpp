#include <iostream>
#include <cstdlib>

#include "operator_c.h"
#include "persist_c.h"


int main() {    
    std::cout << "- Computing operator C..." << std::endl;

    computeC();

    return EXIT_SUCCESS;
}

