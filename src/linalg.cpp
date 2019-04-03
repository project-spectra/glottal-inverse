#include "linalg.h"

#include <iostream>
#include <cstring>

static constexpr size_t length = basis_length();

static auto haar = shared_ptr<gsl_wavelet>(
        gsl_wavelet_alloc(gsl_wavelet_haar, 2),
        gsl_wavelet_free
);

static auto workspace = shared_ptr<gsl_wavelet_workspace>(
        gsl_wavelet_workspace_alloc(length),
        gsl_wavelet_workspace_free
);


static void transform(double x[], double y[], gsl_wavelet_direction dir) {
    memcpy(y, x, length * sizeof(double));
    gsl_wavelet_transform(haar.get(), y, 1, length, dir, workspace.get());
}

static PtrVector getOrSetBasis(BasisMap& stored, size_t k);


void coords(double f[], double u[]) {
    return transform(f, u, gsl_wavelet_forward);
}

void uncoords(double u[], double f[]) {
    return transform(u, f, gsl_wavelet_backward);
}

void normalize(gsl_vector *f) {
    size_t max_index = gsl_blas_idamax(f);
    double max = gsl_vector_get(f, max_index);

    // don't normalize all the way
    gsl_vector_scale(f, .95 / abs(max));
}

PtrVector convoluteBasis(size_t n, size_t k) {
    // one map for the bases in wavelet coordinates
    static BasisMap storedBases;

    // one map for already computed convolutions
    static ConvMap storedConvs;

    PtrVector transConv;

    // check if [ F(n) * F(k) ] is already computed
    // remember that (n, k) is not supposed to be ordered
    auto pair_nk = IndPair(n, k);
    auto pair_kn = IndPair(k, n);

    auto iter_nk = storedConvs.find(pair_nk);
    auto iter_kn = storedConvs.find(pair_kn);
    
    auto nk_exist = (iter_nk != storedConvs.end());
    auto kn_exist = (iter_kn != storedConvs.end());

    if (nk_exist && kn_exist) {
        transConv = iter_nk->second;
    } else if (nk_exist) {
        // if <n,k> but not <k,n> then reassign.
        transConv = storedConvs[pair_kn] = iter_nk->second;
    } else if (kn_exist) {
        // same thing here.
        transConv = storedConvs[pair_nk] = iter_kn->second;
    } else {
        // this branch means that there is no entry.
        // so calculate the convolution~
        
        // get the bases from precomputation
        auto phi_n = getOrSetBasis(storedBases, n);
        auto phi_k = getOrSetBasis(storedBases, n);

        // do the convolution
        auto conv = gsl_vector_alloc(length);

        gsl_vector_memcpy(conv, phi_n.get());
        gsl_vector_mul(conv, phi_k.get());
      
        // the way GSL handles wavelet transforms you can do it in place.
        uncoords(conv->data, conv->data);

        // transfer it to the dynamic storage and free the rest
        transConv = PtrVector(conv, gsl_vector_free);

        // and assign it to reuse dynamically
        storedConvs[pair_nk] = storedConvs[pair_kn] = transConv;
    }

    return transConv;
}

static PtrVector getOrSetBasis(BasisMap& stored, size_t k) {

    PtrVector transBasis;

    // check if F(p) is already computed
    auto iter = stored.find(k);

    if (iter == stored.end()) {
        // the way that GSL wavelet works , you can use a single vector.
        // transform to wavelet
        auto phi = gsl_vector_alloc(length); 
        gsl_vector_set_basis(phi, k);
 
        coords(phi->data, phi->data);

        transBasis = stored[k] = PtrVector(phi, gsl_vector_free);
    } else {
        transBasis = iter->second;
    }

    return transBasis;
}

