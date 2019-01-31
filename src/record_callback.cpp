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
    sample *wptr = &data->recordedSamples[data->frameIndex];
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
            *wptr++ = 0.0;
        }
    } else {
        for (i = 0; i < framesToCalc; ++i) {
            *wptr++ = *rptr++;
        }
    }

    data->frameIndex += framesToCalc;
    return finished;
}



