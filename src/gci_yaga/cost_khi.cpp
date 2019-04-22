#include "gci_yaga_subroutines.h"


double cost_khi(const double mu, const double sigma, const double y)
{
    const double a(y - mu);

    return exp(-a*a / (2*sigma*sigma));
}
