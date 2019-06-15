#include "gci_pd.h"


void removeDC(arma::dvec& vec) {
    vec -= arma::mean(vec);
}
