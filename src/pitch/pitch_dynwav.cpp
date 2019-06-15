#include <cmath>
#include <climits>
#include <vector>
#include "audio.h"
#include "pitch.h"


static constexpr int maxFLWTlevels(10);
static constexpr double maxF(3000.);
static constexpr int differenceLevelsN(3);
static constexpr double maximaThresholdRatio(0.75);



DECL_PITCH(DynWav) {
    std::vector<int> distances(WINDOW_LENGTH, 0);
    std::vector<int> mins(WINDOW_LENGTH, 0);
    std::vector<int> maxs(WINDOW_LENGTH, 0);

    std::vector<double> buffer(WINDOW_LENGTH, 0.);
    std::copy(std::begin(frame), std::end(frame), buffer.begin());

    double f0(-1.);

    int curSamNb(WINDOW_LENGTH);
    int nbMins, nbMaxs;

    double amplitudeThreshold;
    double theDC = 0.0;
    
    // Compute amplitudeThreshold and theDC
    double maxValue(0.), minValue(0.);

    for (const auto& sample : frame) {
        theDC += sample;
        maxValue = std::max(maxValue, sample);
        minValue = std::min(sample, minValue);
    }

    theDC /= (double) WINDOW_LENGTH;
    maxValue -= theDC;
    minValue -= theDC;

    double amplitudeMax = std::max(std::abs(maxValue), std::abs(minValue));
    
    amplitudeThreshold = amplitudeMax * maximaThresholdRatio;

    // Levels, start without downsampling...
    int curLevel(0);
    double curModeDistance(-1.);
    int delta;

    while (true) {
        delta = (double) SAMPLE_RATE / (std::pow<double>(2, curLevel) * maxF);
        if (curSamNb < 2) {
            break;
        }

        double dv, previousDV(-1000);

        nbMins = nbMaxs = 0;
        int lastMinIndex(-1000000);
        int lastMaxIndex(-1000000);
        bool findMax(false);
        bool findMin(false);

        for (int i = 2; i < curSamNb; ++i) {
            double si = buffer[i] - theDC;
            double si1 = buffer[i - 1] - theDC;

            if (si1 <= 0 && si > 0) findMax = true;
            if (si1 >= 0 && si < 0) findMin = true;

            dv = si - si1;

            if (previousDV > -1000) {
                if (findMin && previousDV < 0 && dv >= 0) { // minimum
                    if (std::abs(si) >= amplitudeThreshold
                            && i > lastMinIndex + delta) {
                        mins[nbMins++] = i;
                        lastMinIndex = i;
                        findMin = false;
                    }
                }
                if (findMax && previousDV > 0 && dv <= 0) { // maximum
                    if (std::abs(si) >= amplitudeThreshold
                            && i > lastMaxIndex + delta) {
                        maxs[nbMaxs++] = i;
                        lastMaxIndex = i;
                        findMax = false;
                    }
                }
            }

            previousDV = dv;
        }

        if (nbMins == 0 && nbMaxs == 0) {
            break;
        }

        int d;
        for (int i = 0; i < nbMins; ++i) {
            for (int j = 1; j < differenceLevelsN; ++j) {
                if (i + j < nbMins) {
                    d = std::abs(mins[i] - mins[i + j]);

                    distances[d] = distances[d] + 1;
                }
            }
        }

        int bestDistance(-1);
        int bestValue(-1);
        for (int i = 0; i < curSamNb; ++i) {
            int summed(0);
            for (int j = -delta; j <= delta; ++j) {
                if (i + j >= 0 && i + j < curSamNb)
                    summed += distances[i + j];
            }

            if (summed == bestValue) {
                if (i == 2 * bestDistance)
                    bestDistance = i;
            }
            else if (summed > bestValue) {
                bestValue = summed;
                bestDistance = i;
            }
        }

        // Averaging
        double distAvg = 0.0;
        double nbDists = 0;
        for (int j = -delta; j <= delta; j++) {
            if (bestDistance + j >= 0 && bestDistance + j < WINDOW_LENGTH) {
                int nbDist = distances[bestDistance + j];
                if (nbDist > 0) {
                    nbDists += nbDist;
                    distAvg += (bestDistance + j) * nbDist;
                }
            }
        }
        // This is our mode distance:
        distAvg /= nbDists;

        // Continune the levels?
        if (curModeDistance > -1.) {
            double similarity = std::abs(distAvg * 2 - curModeDistance);
            if (similarity <= 2 * delta) {
                f0 = (double) SAMPLE_RATE / (std::pow<double>(2, curLevel - 1) * curModeDistance);
                break;
            }
        }

        curModeDistance = distAvg;

        curLevel++;
        if (curLevel >= maxFLWTlevels) {
            break;
        }

        if (curSamNb < 2) {
            break;
        }

        std::vector<double> newBuffer(buffer);
        if (curSamNb == distances.size()) {
            newBuffer.resize(curSamNb / 2, 0.);
        }
        for (int i = 0; i < curSamNb / 2; ++i) {
            newBuffer[i] = (buffer[2 * i] + buffer[2 * i + 1]) / 2.;
        }
        
        buffer = std::move(newBuffer);
        curSamNb /= 2;
    }

    *f0res = f0;
    *T0res = 1. / f0;

    return f0 != -1.;
}
