#ifndef GCI_PD_H
#define GCI_PD_H


#include <armadillo>
#include <valarray>

using valarray = std::valarray<double>;

void gci_pd(const valarray& md, std::vector<int>& GCIs);

void reduceVTI(arma::dvec& vec);
void teager(arma::dvec& vec);         // Unused for noisy speech,
void reduceDynRange(arma::dvec& vec); // emphasizes discontinuities
void removeDC(arma::dvec& vec);

arma::cx_vec symSignal(const arma::dvec& vec);

void fft_radius(arma::cx_vec& vec, double radius = 1.);
void ifft_radius(arma::cx_vec& vec, double radius = 1.);

arma::dvec phaseDifference(const arma::cx_vec& vec);
double atan4(double y, double x);

void selectPeaks(arma::dvec& vec, std::vector<int>& peaks, double threshold);


#endif // GCI_PD_H
