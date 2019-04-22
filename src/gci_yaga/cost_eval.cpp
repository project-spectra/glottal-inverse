#include "gci_yaga_subroutines.h"

double cost_eval(const valarray& lambda, const valarray& costs)
{
    return (lambda * costs).sum();
}
