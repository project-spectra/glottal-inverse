#include "audio_in.h"
#include "constants.h"


int recordCallback(
        const void *inputBuffer,
        void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
) {
    auto *data = static_cast<window_data *>(userData);
    auto *rptr = static_cast<const float *>(inputBuffer);
    auto *wptr = &data->recordedSamples[data->frameIndex];

    size_t framesToCalc, i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer;
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if (framesLeft < framesPerBuffer) {
        framesToCalc = framesLeft;
        finished = paComplete;
    } else {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if (inputBuffer == nullptr) {
        for (i = 0; i < framesToCalc; ++i) {
            *wptr++ = 0.;
        }
    } else {
        for (i = 0; i < framesToCalc; ++i) {
            *wptr++ = static_cast<double>(*rptr++);
        }
    }

    data->frameIndex += framesToCalc;
    return finished;
}



