#ifndef AUDIO_IN_H
#define AUDIO_IN_H


#include <atomic>
#include <valarray>

#include <soundio/soundio.h>

using valarray = std::valarray<double>;

struct audio_s {
    struct SoundIo *soundio;
    struct SoundIoDevice *device;
    struct SoundIoInStream *inStream;
    struct SoundIoRingBuffer *buffer;
    volatile std::atomic_bool running;
};

extern int SAMPLE_RATE;
extern int AUDIO_FORMAT;
extern int WINDOW_LENGTH;

void readCallback(struct SoundIoInStream *inStream, int frame_count_min, int frame_count_max);
void overflowCallback(struct SoundIoInStream *inStream);

bool initAudio(audio_s& audio);
void destroyAudio(audio_s& audio);

bool startStream(audio_s& audio);
bool pauseStream(audio_s& audio, bool pause = true);

inline bool resumeStream(audio_s& audio) {
    return pauseStream(audio, false);
}

bool hasAtLeastOneWindow(audio_s& audio);
void loadWindow(audio_s& audio, valarray& md);

#endif // AUDIO_IN_H
