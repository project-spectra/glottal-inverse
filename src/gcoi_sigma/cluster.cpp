#include <climits>
#include "gcoi_sigma_subs.h"


template<int dir>
int gcoi_sigma_cluster(const arma::dmat& fv, arma::ivec& in)
{
    // Determine clusters 
    arma::gmm_diag model;

    bool status = model.learn(
            fv, SIGMA_nclust,
            arma::maha_dist, arma::random_subset,
            20,    // km_iter
            15,    // em_iter
            10. * std::numeric_limits<double>::epsilon(), 
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
        I = arma::index_min(mm.col(0));
    }
    else {
        I = arma::index_max(mm.col(0));
    }

    // Find log likelihoods for each cluster
    arma::dmat ll = arma::zeros<arma::dmat>(SIGMA_nclust, fv.n_cols); // k x n

    for (int i = 0; i < SIGMA_nclust; ++i) {
        model.reset(3, 1);
        model.set_params(mm.row(i), vv.row(i), ww);

        bool status = model.learn(
            fv.row(i), 1,
            arma::maha_dist, arma::keep_existing,
            0,
            1,
            10. * std::numeric_limits<double>::epsilon(), 
            false
        );

        if (!status) {
            std::cerr << "Learning failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        ll.row(i) = model.log_p(fv.row(i));
        
        std::cout << "cluster " << i << std::endl;
        std::cout << ll.row(i) << std::endl;
    }

    // Find which cluster each feature vector belongs to
    in.resize(fv.n_cols);

    for (int n = 0; n < fv.n_cols; ++n) {
        in(n) = arma::index_max(ll.col(n));
    }

    return I;
    
}

template int gcoi_sigma_cluster<-1>(const arma::dmat&, arma::ivec&);
template int gcoi_sigma_cluster<1>(const arma::dmat&, arma::ivec&);

