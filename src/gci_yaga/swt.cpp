#include <cmath>
#include "gci_yaga.h"
#include <cstdio>

void computeSWT(const valarray& u, valarray& p)
{
    const int N(u.size());
    const int J(log2(N) - 1);

    constexpr int j0(1);
    const int j1(std::min(3, (int) J));

    std::vector<valarray> d(j1 + 1, valarray(u));
    
    int j;

    p.resize(N, 1.);

    valarray Fj = { M_SQRT1_2, M_SQRT1_2 };
    valarray Gj = { -M_SQRT1_2, M_SQRT1_2 };

    valarray Aj = u; 
    valarray Dj;

    for (j = j0; j <= j1; ++j) {

        valarray Ajp1(N), Djp1(N);
        
        for (int n = 0; n < N; ++n) {
            Ajp1[n] = 0.;
            for (int m = 0; m < Fj.size() && m <= n; ++m) {
                Ajp1[n] += Aj[n - m] * Fj[m];
            }
           
            Djp1[n] = 0.;
            for (int m = 0; m < Gj.size() && m <= n; ++m) {
                Djp1[n] += Aj[n - m] * Gj[m];
            }
        }

        Aj = Ajp1;
        Dj = Djp1;

        // Upsample the filters by 2
        
        valarray Fjp1(2*Fj.size(), 0.);
        valarray Gjp1(2*Gj.size(), 0.);

        Fjp1[std::slice(0, Fj.size(), 2)] = Fj;
        Gjp1[std::slice(0, Gj.size(), 2)] = Gj;

        p *= pow(Dj, 1. / (double) j1);
    }

    // half-wave rectifying
    for (int n = 0; n < N; ++n) {
        if (!std::isnormal(p[n])) {
            p[n] = 0.;
        }
    }

}
