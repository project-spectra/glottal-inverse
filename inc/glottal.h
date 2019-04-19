#ifndef GLOTTAL_H
#define GLOTTAL_H


#include <vector>
#include <utility>

#include <gsl/gsl_vector.h>

struct VPFrame {
    double t;    // GCI (s)
    bool valid;  // false if not actually a cycle

    double NAQ;  // Normalized amplitude quotient
    double QOQ;  // Quasi open quotient
    double H1H2; // Difference in H1-H2 amplitude
    double HRF;  // Harmonic richness factor
    double PSP;  // Parabolic spectral parameter
};

typedef std::vector<VPFrame> GlottalParameters;

GlottalParameters estimateGlottal(gsl_vector *gf, gsl_vector *gfd, const double fs, const std::vector<size_t>& gci);

std::pair<size_t, size_t> findAmid_t(gsl_vector *glot_adj, double Amid, size_t Tz);


#endif // GLOTTAL_H
