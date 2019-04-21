#include "gci_yaga.h"

double cost_eval(const valarray& lambda, const valarray& costs)
{
    return (lambda * costs).sum();
}
