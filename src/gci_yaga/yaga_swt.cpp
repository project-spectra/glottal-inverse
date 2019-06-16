#include <cmath>
#include <algorithm>
#include "gci_yaga.h"
#include "gnuplot.h"


// bior1.1 decomposition low-pass filter
static const valarray Lo_D = {    
    0.7071067811865476,
    0.7071067811865476,
};

// bior1.1 decomposition high-pass filter
static const valarray Hi_D = {
    -0.7071067811865476,
    0.7071067811865476,
};

static void swt_conv_per(const valarray& input, const valarray& filter, valarray& output, const int fstep);
static void swt_step(const valarray& input, const valarray& filter, const int level, valarray& output);

#define swt_step_a(in, lv, out) swt_step(in, Lo_D, lv, out)
#define swt_step_d(in, lv, out) swt_step(in, Hi_D, lv, out)

void computeSWT(const valarray& u, valarray& p)
{
    const int Nu(u.size());
    const int J(std::floor(log2(Nu)));

    const int NU(std::pow(2, J + 1));

    constexpr int j0(1);
    const int j1(std::min(3, J));

    valarray data;
    data.resize(NU, 0.0);
    data[std::slice(0, Nu, 1)] = u;

    valarray cD, cA;

    p.resize(Nu, 1.); 

    for (int j = j0; j <= j1; ++j) {
        // Decompose Detail coefficients
        swt_step_d(data, j, cD);

        // Decompose Approximation coefficients
        swt_step_a(data, j, cA);

        data = cA;

        p *= valarray(cD[std::slice(0, Nu, 1)]);
    }

    for (int i = 0; i < Nu; ++i) {
        if (!std::isnormal(p[i]) || p[i] < 0.) {
            p[i] = 0.;
        }
    }
}

static void swt_step(const valarray& input, const valarray& filter, const int level, valarray& output)
{
    const int stride = std::pow(2, level - 1);

    valarray up_filter;

    up_filter.resize(filter.size() * stride, 0.0); 
    up_filter[std::slice(0, filter.size(), stride)] = filter;

    output.resize(input.size(), 0.0);
    swt_conv_per(input, up_filter, output, stride);
}

static void swt_conv_per(const valarray& input, const valarray& filter, valarray& output, const int fstep)
{
    const int N(input.size());
    const int F(filter.size());
    
    int i = F / 2;
    int o = 0;

    int j;

    for (; i < F && i < N; ++i, ++o) {
        double sum(0.0);
        int k_start(0);

        for (j = 0; j <= i; j += fstep) {
            sum += filter[j] * input[i - j];
        }

        if (fstep > 1) {
            k_start = j - (i + 1);
        }

        while (j < F) {
            for (int k = k_start; k < N && j < F; k += fstep, j += fstep) {
                sum += filter[j] * input[N - 1 - k];
            }
        }

        output[o] = sum;
    }

    for (; i < N; ++i, ++o) {
        double sum(0.0);
        
        for (j = 0; j < F; j += fstep) {
            sum += input[i - j] * filter[j];
        }

        output[o] = sum;    
    }

    for (; i < F && i < N + F / 2; ++i, ++o) {
        double sum(0.0);
        int k_start(0);

        j = 0;
        while (i - j >= N) {
            for (int k = k_start; k < N && i - j >= N; ++k, ++j) {
                sum += filter[i - N - j] * input[k];
            }
        }

        if (fstep > 1) {
            j += (fstep - j % fstep) % fstep;
        }

        for (; j <= i; j += fstep) {
            sum += filter[j] * input[i - j];
        }

        if (fstep > 1) {
            k_start = j - (i + 1);
        }

        while (j < F) {
            for (int k = k_start; k < N && j < F; k += fstep, j += fstep) {
                sum += filter[j] * input[N - 1 - k];
            }
        }

        output[o] = sum;
    }

    for (; i < N + F / 2; ++i, ++o) {
        double sum(0.0);

        j = 0;
        while (i - j >= N) {
            for (int k = 0; k < N && i - j >= N; ++k, ++j) {
                sum += filter[i - N - j] * input[k];
            }
        }

        if (fstep > 1) {
            j += (fstep - j % fstep) % fstep;
        }

        for (; j < F; j += fstep) {
            sum += filter[j] * input[i - j];
        }

        output[o] = sum;
    }
}
