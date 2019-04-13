#include "linalg.h"

shared_ptr<BasisMap> storedBases = std::make_shared<BasisMap>();


std::recursive_mutex basisLock;


gsl_vector *getBasis(size_t k) {

    basisLock.lock();
    gsl_vector *phi = storedBases->at(k).get();
    basisLock.unlock();

    // if it doesn't exist, compute
    if (phi == nullptr || phi->size == 0) {
        phi = gsl_vector_alloc(basisLength);

        gsl_vector_set_basis(phi, k);

        projForward(phi);
       
        basisLock.lock();
        storedBases->at(k) = VecPtr(phi, vecFree);
        basisLock.unlock();
    }

    return phi;
}
