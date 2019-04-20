#include <array>
#include <cmath>
#include <tuple>
#include <gsl/gsl_blas.h>
#include "glottal.h"


static constexpr double F0min = 20.;
static constexpr double F0max = 600.;

static constexpr double QOQ_level = 0.5;
static constexpr size_t T0_num = 3;
static constexpr size_t min_harm_num = 5;
static constexpr double HRF_freq_max = 5000.;
static constexpr size_t PSP_fft_size = 2048;

using namespace std;

GlottalParameters estimateGlottal(gsl_vector *gf, gsl_vector *gfd, const double fs, const vector<size_t>& gci)
{ 
    const size_t N(gci.size());
    const size_t glot_shift = round(.5 / 1000. * fs);

    GlottalParameters result(N);

    size_t n;
    size_t start, stop, stop2;
    size_t T0;
    double F0;

    size_t gf_seg_start, gf_seg_end;
    gsl_vector *gf_seg;
    gsl_vector *gf_seg_comp;
    gsl_vector *gfd_seg;
    
    for (n = 0; n < N; ++n) {
        size_t gci_n = gci[n];

        // Get glottal pulse compensated for zero-line drift
        if (n == 1) {
            start = 1;
            stop = gci_n;
            T0 = gci[n+1] - gci_n;
        } else {
            start = gci[n-1];
            stop = gci_n;
            T0 = gci_n - gci[n-1];
        }
        F0 = fs / (double) T0;

        VPFrame frame;
        frame.t = gci_n / fs;
    
        // Safety measures
        if (!isfinite(F0) || T0 == 0 || F0 < F0min || F0 > F0max) {
            frame.valid = false;
            result[n] = frame;
            continue;
        }

        frame.valid = true;
        
        auto gfSegView = gsl_vector_subvector(gf, start, stop - start);
        gf_seg = &gfSegView.vector;

        // Compensate for zero-line drift in the glottal flow pulse
        gf_seg_comp = gsl_vector_alloc(stop - start);
        gsl_vector_memcpy(gf_seg_comp, gf_seg);

        if (start != stop) {
            gf_seg_start = gsl_vector_get(gf_seg, 0);
            if (gf_seg->size > 1) {
                gf_seg_end = gsl_vector_get(gf_seg, stop - start - 1);
                
                // interpolate line and subtract
                double a((double) (gf_seg_end - gf_seg_start) / (double) (stop - start));
                double b(gf_seg_start);

                for (size_t k = 0; k < stop - start; ++k) {
                    *gsl_vector_ptr(gf_seg_comp, k) -= a * k + b;
                }
            } else {
                gsl_vector_add_constant(gf_seg_comp, -gf_seg_start);
            }
        }
        
        if (stop + glot_shift <= gfd->size) {
            stop2 = stop + glot_shift;
        } else {
            stop2 = stop;
        }

        auto gfdSegView = gsl_vector_subvector(gfd, start, stop2 - start);
        gfd_seg = &gfdSegView.vector;

        // Get NAQ and QOQ
        size_t d_peakInd = gsl_blas_idamax(gfd_seg);
        double d_peak = abs(gsl_vector_get(gfd_seg, d_peakInd));

        size_t max_idx = gsl_vector_max_index(gf_seg_comp);
        double f_ac = gsl_vector_get(gf_seg_comp, max_idx);
        double Amid = f_ac * QOQ_level;
        size_t T1, T2;
        std::tie(T1, T2) = findAmid_t(gf_seg_comp, Amid, max_idx);

        frame.NAQ = (f_ac / d_peak) / (double) T0;
        frame.QOQ = ((double) T2 - (double) T1) / (fs / F0);

        // TODO: everything else

        result[n] = frame;

        gsl_vector_free(gf_seg_comp);
    }

    return result;
}

std::pair<size_t, size_t> findAmid_t(gsl_vector *glot_adj, double Amid, size_t Tz)
{
    size_t T1(0);
    size_t T2(0);

    if (Tz != 0) {
        size_t n(Tz);

        while (gsl_vector_get(glot_adj, n) > Amid && n > 3) {
            n--;
        }
        T1 = n;
        n = Tz;

        while (gsl_vector_get(glot_adj, n) > Amid && n < glot_adj->size - 2) {
            n++;
        }
        T2 = n;
    }

    return { T1, T2 };
}
