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
    sample *wptr = data->recordedSamples + data->frameIndex;
    long framesToCalc;
    long i;
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
        *wptr++ = inputBuffer ? *rptr++ : 0.0;
    }

    data->frameIndex += framesToCalc;
    return finished;
}



