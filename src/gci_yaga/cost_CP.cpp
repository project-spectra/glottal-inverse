#include "audio.h"
#include "gci_yaga_subroutines.h"


double cost_CP(const valarray& segmentNorms, const double maxSegmentNorm, const int r)
{
    return segmentNorms[r] / maxSegmentNorm;
}
