#include "gci_yaga_subroutines.h"


double cost_CP(const valarray& segmentNorms, const double maxSegmentNorm, const size_t r)
{
    return segmentNorms[r] / maxSegmentNorm;
}
