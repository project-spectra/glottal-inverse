#include "linalg.h"

#include <iostream>
#include <cstring>
#include <mutex>


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

static double *getBasis(BasisList& stored, size_t k);


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

static std::mutex basis;

double convoluteBasis(size_t n, size_t k, size_t mu) {
    std::lock_guard<std::mutex> lock(basis);

    // one map for the bases in wavelet coordinates
    static BasisList storedBases(length);

    // one map for already computed convolutions
    static ConvMap storedConvs;

    // check if [ F(n) * F(k) ] is already computed
    // remember that (n, k) is unordered in the map
    
    auto pair_nk = IndPair(n, k);
    auto iter_nk = storedConvs.find(pair_nk);
    
    auto nk_exists = (iter_nk != storedConvs.end());

    if (nk_exists) {
        return iter_nk->second[mu];
    } else {
        // this branch means that there is no entry.
        // so calculate the convolution~
        
        // get the bases from precomputation
        auto phi_n = getBasis(storedBases, n);
        auto phi_k = getBasis(storedBases, n);

        // do the convolution
        auto conv = make_unique<double[]>(length);

        for (size_t mu = 0; mu < length; ++mu) {
            conv[mu] = phi_n[mu] * phi_k[mu];
        }
      
        // the way GSL handles wavelet transforms you can do it in place.
        uncoords(conv.get(), conv.get());

        double res = conv[mu];

        // and assign it to reuse dynamically
        storedConvs[pair_nk] = std::move(conv);

        return res;
    }
}

static double *getBasis(BasisList& stored, size_t k) {

    // check if F(p) is already computed
    double *transBasis = stored[k].get();

    if (transBasis == nullptr) {
        // the way that GSL wavelet works , you can use a single vector.
        // transform to wavelet
        auto phi = make_unique<double[]>(length);
        
        for (size_t mu = 0; mu < length; ++mu) {
          phi[mu] = (mu != k) ? 0. : 1.;
        }
 
        coords(phi.get(), phi.get());

        transBasis = phi.get();
        stored[k] = std::move(phi);
    }

    return transBasis;
}




