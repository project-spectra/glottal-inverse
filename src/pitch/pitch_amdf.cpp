#include "pitch.h"
#include "gsl_util.h"


static constexpr double minFrequency(80.);
static constexpr double maxFrequency(800.);
static constexpr double ratio(5.0);
static constexpr double sensitivity(0.1);

static const double minPeriod = round(SAMPLE_RATE / maxFrequency + .5);
static const double maxPeriod = round(SAMPLE_RATE / minFrequency + .5);


DECL_PITCH(AMDF) {

    double f0(-1);
    double minVal(HUGE_VAL);  // +inf
    double maxVal(-HUGE_VAL); // -inf

    const size_t maxShift(frame->size);

    static_vector2(amd, maxShift);

    for (size_t i = 0; i < maxShift; ++i) {

        auto frame1 = gsl_vector_alloc(maxShift - i);
        memcpy(frame1->data, frame->data, (maxShift - i) * sizeof(double));

        auto frame2 = gsl_vector_subvector(frame, i, maxShift - i);
    
        gsl_vector_sub(frame1, &frame2.vector); // frame1 - frame2
        double aSum = gsl_blas_dasum(frame1);   // sum || frame1 - frame2 ||
        gsl_vector_free(frame1);

        gsl_vector_set(amd, i, aSum); 
    }
    
    auto amdSub = gsl_vector_subvector(amd, minPeriod, maxPeriod - minPeriod);

    gsl_vector_minmax(&amdSub.vector, &minVal, &maxVal);
    
    size_t cutoff = round((sensitivity * (maxVal - minVal)) + minVal);
    size_t j = minPeriod;

    while (j <= (size_t) maxPeriod && (gsl_vector_get(amd, j) > cutoff)) {
        j++;
    }

    size_t searchLength(minPeriod / 2);
    minVal = gsl_vector_get(amd, j);
    
    size_t minPos(j);
    size_t i(j);

    while ((i < j + searchLength) && (i <= maxPeriod)) {
        i++;

        double cur(gsl_vector_get(amd, i));

        if (cur < minVal) {
            minVal = cur;
            minPos = i;
        }
    }

    if (round(gsl_vector_get(amd, minPos) * ratio) < maxVal) {
        f0 = SAMPLE_RATE / minPos;
    }

    *f0res = f0;
    *T0res = 1. / f0;

    return f0 != -1;

}
