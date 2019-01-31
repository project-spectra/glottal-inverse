#ifndef AUDIO_IN_H
#define AUDIO_IN_H


#include <portaudio.h>
#include <stddef.h>

typedef float sample;

struct window_data {
    int frameIndex;
    int maxFrameIndex;
    sample *recordedSamples;
};

int recordCallback(
        const void *inputBuffer,
        void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
);

bool openStream(PaStream **stream, window_data *data);

bool recordWindow(PaStream *stream);


#endif // AUDIO_IN_H
