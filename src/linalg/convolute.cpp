#include "linalg.h"


unique_ptr<ConvMap> storedConvs = std::make_unique<ConvMap>(basisLength * basisLength);


std::recursive_mutex convLock;


const Vector& getConvoluted(size_t n, size_t k) {
    std::lock_guard<std::recursive_mutex> guard(convLock);

    // mimic an unordered pair
    std::tie(n, k) = std::minmax(n, k);
  
    auto& conv = storedConvs[n * basisLength + k];

    // if it doesn't exist, compute
    if (!conv) {

        if (n == k) {
            auto& phi = getBasis(n);

            conv = phi;
            conv *= phi;
        } else {
            // get both
            auto& phi_n = getBasis(n);
            auto& phi_k = getBasis(k);

            // convolute
            conv = phi_n;
            conv *= phi_k;
        }
        
        // uncords
        conv.uncoords();
    }

    return conv;
}
