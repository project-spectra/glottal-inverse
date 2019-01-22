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
    window_data *data = (window_data *) userData;
    const sample *rptr = (const sample *) inputBuffer;
    sample *wptr = data->recordedSamples + (data->frameIndex * NUM_CHANNELS);
    long framesToCalc;
    long i, k;
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

    for (i = 0; i < framesToCalc; ++i) {
        for (k = 0; k < NUM_CHANNELS; ++k) {
            *wptr++ = inputBuffer ? *rptr++ : 0.0;
        }
    }

    data->frameIndex += framesToCalc;
    return finished;
}



