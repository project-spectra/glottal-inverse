#include <set>
#include <utility>
#include <iostream>
#include "audio.h"
#include "lf_fitting.h"


static int find_tp0(const valarray& u, int te);
static void fit_alpha_mmse(const valarray& ro, const lf_params& lf, double a0, double *mmse, double *alpha);
static void fit_epsilon_mmse(const valarray& rr, const lf_params& lf, double e0, double *mmse, double *epsilon);


bool lf_fit_period(const valarray& u, int te, double T0, lf_params& out)
{
    // Parameters for the LF model.
    lf_params lf;

    // Fundamental period in samples
    lf.N = std::round(T0 * SAMPLE_RATE);

    // Find amplitude of glottal closure instant te
    lf.Ee = std::abs(u[te]);

    // Find tp0 which is the first zero-crossing point before te
    int tp0 = find_tp0(u, te);
    if (tp0 < 0) {
        return false;
    }

    valarray ro, rr;

    // Find optimal t0
    int t0opt = te - .15 * lf.N;
    double a0opt = 0;
    double min_mmse = HUGE_VAL;

    for (int t0 = te - lf.N; t0 < te - .15 * lf.N; ++t0) {
        ro = u[std::slice(t0, te - t0, 1)];
        lf.Tp = (double) (tp0 - t0) / lf.N;
        lf.Te = (double) (te - t0) / lf.N;

        double a0 = 0;
        while (a0 <= 100) {
            double mmse, alpha;

            fit_alpha_mmse(ro, lf, a0, &mmse, &alpha);

            if (mmse < min_mmse) {
                min_mmse = mmse;
                t0opt = t0;
                a0opt = alpha;
            }

            a0 += 4;
        }
    }
    
    // Update ro, Tp, Te according to t0opt
    ro = u[std::slice(t0opt, te - t0opt, 1)];
    double Tp0 = (double) (tp0 - t0opt) / lf.N;
    lf.Te = (double) (te - t0opt) / lf.N;

    // Find optimal Tp
    double Tpopt = Tp0;

    double Tp_shf = -0.05;
    while (Tp_shf <= 0.05) {
        lf.Tp = Tp0 + Tp_shf;
        
        double mmse, alpha;

        fit_alpha_mmse(ro, lf, a0opt, &mmse, &alpha);

        if (mmse < min_mmse) {
            min_mmse = mmse;
            Tpopt = lf.Tp;
        }

        Tp_shf += 0.02;
    }

    lf.Tp = Tpopt;

    // Find optimal Ta
    rr = u[std::slice(te, lf.N, 1)];

    double e0opt = 0;
    
    double e0 = 0;
    while (e0 <= 200) {
        double mmse, epsilon;

        fit_epsilon_mmse(rr, lf, e0, &mmse, &epsilon);

        if (mmse < min_mmse) {
            min_mmse = mmse;
            e0opt = epsilon;
        }

        e0 += 4;
    }
    
    // Calculate Ta by e0opt
    lf.Ta = (1. - std::exp(-e0opt * (1. - lf.Te))) / e0opt;

    out = lf;

    return true;
}

int find_tp0(const valarray& u, int te) {
    int tp0 = te;
    bool found = false;

    while (tp0 > 0 && !found) {
        if (u[tp0] * u[tp0-1] < 0) {
            found = true;
        }
        tp0--;
    }

    return found ? tp0 : -1;
}

void fit_alpha_mmse(const valarray& ro, const lf_params& lf, double a0, double *mmse, double *alpha) {
    static std::default_random_engine rdgen;

    const int N = ro.size();

    // Run the ELK on this segment
    auto state = lf_fit_alpha_init(rdgen, a0);
    for (int k = 1; k < N; ++k) {
        lf_fit_alpha_update(k, lf, state);
    }
    
    *alpha = state.ak_1;

    // Reconstruct the LF model from the new alpha parameter
    valarray ro_est(N);
    for (int k = 0; k < N; ++k) {
        ro_est[k] = lf_ho(*alpha, k, lf);
    }

    *mmse = lf_mmse(ro, ro_est);
}

void fit_epsilon_mmse(const valarray& rr, const lf_params& lf, double e0, double *mmse, double *epsilon) {
    static std::default_random_engine rdgen;

    const int N = rr.size();

    // Run the ELK on this segment
    auto state = lf_fit_epsilon_init(rdgen, e0);
    for (int k = 1; k < N; ++k) {
        lf_fit_epsilon_update(k, lf, state);
    }
    
    *epsilon = state.ek_1;

    // Reconstruct the LF model from the new alpha parameter
    valarray rr_est(N);
    for (int k = 0; k < N; ++k) {
        rr_est[k] = lf_ho(*epsilon, k, lf);
    }

    *mmse = lf_mmse(rr, rr_est);
}
