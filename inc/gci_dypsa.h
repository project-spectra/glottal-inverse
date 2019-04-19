#ifndef GCI_DYPSA_H
#define GCI_DYPSA_H


#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

#include <gsl/gsl_vector.h>

using std::tie;
using std::tuple;
using std::pair;
using std::vector;

#include "constants.h"
#include "filter.h"
#include "lpc.h"


static constexpr double dy_cpfrac = 0.3;     // presumed closed phase fraction of larynx cycle
static constexpr double dy_cproj = 0.2;      // cost of projected candidate
static constexpr double dy_cspurt = -0.45;   // cost of a talkspurt
static constexpr bool dy_dopsp = true;       // Use phase slope projection (1) or not (0)?
static constexpr double dy_ewdly = 0.0008;   // window delay for energy cost function term [~ energy peak delay from closure] (sec)
static constexpr double dy_ewlen = 0.003;    // window length for energy cost function term (sec)
static constexpr double dy_ewtaper = 0.001;  // taper length for energy cost function window (sec)
static constexpr double dy_fwlen = 0.00045;  // window length used to smooth group delay (sec)
static constexpr double dy_fxmax = 500;      // max larynx frequency (Hz)
static constexpr double dy_fxmin = 50;       // min larynx frequency (Hz)
static constexpr double dy_fxminf = 60;      // min larynx frequency (Hz) [used for Frobenius norm only]
static constexpr double dy_gwlen = 0.0030;   // group delay evaluation window length (sec)
static constexpr double dy_lpcdur = 0.020;   // lpc analysis frame length (sec)
static constexpr size_t dy_lpcn = 2;         // lpc additional poles
static constexpr double dy_lpcnf = 0.001;    // lpc poles per Hz (1/Hz)
static constexpr double dy_lpcstep = 0.010;  // lpc analysis step (sec)
static constexpr size_t dy_nbest = 5;        // Number of NBest paths to keep
static constexpr double dy_preemph = 50;     // pre-emphasis filter frequency (Hz) (to avoid preemphasis, make this very large)
static constexpr double dy_spitch = 0.2;     // scale factor for pitch deviation cost
static constexpr double dy_wener = 0.3;      // DP energy weighting
static constexpr double dy_wpitch = 0.5;     // DP pitch weighting
static constexpr double dy_wslope = 0.1;     // DP group delay slope weighting
static constexpr double dy_wxcorr = 0.8;     // DP cross correlation weighting
static constexpr double dy_xwlen = 0.01;     // cross-correlation length for waveform similarity (sec)


void gci_dypsa(gsl_vector *signal);

// [tew, sew, y, toff]
tuple<vector<pair<size_t, double>>, gsl_vector *, double>
    extractGroupDelayEpoch(gsl_vector *u, const double fs);

// [tew, sew]
vector<pair<size_t, double>>
    negativeZeroCrossings(gsl_vector *y);


#endif // GCI_DYPSA_H
