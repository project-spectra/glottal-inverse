#include <climits>
#include "gcoi_sigma_subs.h"


template<int dir>
int gcoi_sigma_cluster(const arma::dmat& fv, arma::ivec& in)
{
    // Determine clusters 
    arma::gmm_diag model;

    bool status = model.learn(
            fv.t(), SIGMA_nclust,
            arma::maha_dist, arma::random_subset,
            100,    // km_iter
            100,    // em_iter
            arma::var(fv.col(0)) / 100.,
            false
    );

    if (!status) {
        std::cerr << "Learning failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    const arma::dmat mm = model.means;
    const arma::dmat vv = model.dcovs;
    const arma::drowvec ww = model.hefts;
    
    // Find cluster with lowest/highest crmp sum 
    int I;

    if (dir < 0) {
        I = arma::index_min(mm.row(0));
    }
    else {
        I = arma::index_max(mm.row(0));
    }

    // Find log likelihoods for each cluster
    arma::dmat ll = arma::zeros<arma::dmat>(fv.n_rows, SIGMA_nclust); // k x n

    for (int i = 0; i < SIGMA_nclust; ++i) {
        model.set_params(mm.row(i), vv.row(i), ww);

        bool status = model.learn(
            fv.col(i).t(), 1,
            arma::maha_dist, arma::keep_existing,
            0,
            1,
            1. / (double) (fv.n_rows * fv.n_rows),
            false
        );

        if (!status) {
            std::cerr << "Learning failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        ll.col(i) = model.log_p(fv.col(i).t()).t();
    }

    // Find which cluster each feature vector belongs to
    in.resize(fv.n_rows);

    for (int n = 0; n < fv.n_rows; ++n) {
        in(n) = arma::index_max(ll.row(n));
    }

    return I;
    
}

template int gcoi_sigma_cluster<-1>(const arma::dmat&, arma::ivec&);
template int gcoi_sigma_cluster<1>(const arma::dmat&, arma::ivec&);

