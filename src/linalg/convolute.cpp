#include "linalg.h"


shared_ptr<ConvMap> storedConvs = std::make_shared<ConvMap>();


std::recursive_mutex convLock;


gsl_vector *getConvoluted(size_t n, size_t k) {
    // mimic an unordered pair
    std::tie(n, k) = std::minmax(n, k);
   
    convLock.lock();
    gsl_vector *conv = storedConvs->at(n * basisLength + k).get();
    convLock.unlock();

    // if it doesn't exist, compute
    if (conv == nullptr) {
        std::lock_guard<std::recursive_mutex> guard(convLock);

        conv = gsl_vector_alloc(basisLength);

        // get both
        auto phi_n = getBasis(n);
        auto phi_k = getBasis(k);

        // convolute
        gsl_vector_memcpy(conv, phi_n);
        gsl_vector_mul(conv, phi_k);

        projBackward(conv);

        storedConvs->at(n * basisLength + k) = VecPtr(conv, vecFree);
    }

    return conv;
}
