#include "linalg.h"

unique_ptr<BasisMap> storedBases = std::make_unique<BasisMap>(basisLength);


std::recursive_mutex basisLock;

const Vector& getBasis(size_t k) {
    std::lock_guard<std::recursive_mutex> guard(basisLock);

    auto& phi = storedBases[k];

    // if it doesn't exist, compute
    if (!phi) {
        phi.reset(basisLength);
        phi.basis(k);
        phi.coords();
    }

    return phi;
}
