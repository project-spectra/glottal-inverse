#ifndef AUDIO_BE_SOUNDIO_H
#define AUDIO_BE_SOUNDIO_H

#ifndef __ANDROID__

#include <atomic>
#include <soundio/soundio.h>

#include "audio.h"

class SoundIoAudioBackend : public AudioBackend
{
public:
    virtual ~SoundIoAudioBackend();

    virtual const char *getName();
    virtual bool isRunning();

    virtual int getSampleRate();
    virtual int getWindowLength();

    virtual bool hasAtLeastOneWindow();
    virtual void loadWindow(valarray& md);

    virtual bool initAudio(double targetLength);
    virtual void destroyAudio();

    virtual bool startStream();
    virtual void stopStream();

private:
    static void readCallback(struct SoundIoInStream *inStream, int frame_count_min, int frame_count_max);
    static void overflowCallback(struct SoundIoInStream *inStream);

    int m_windowLength;

    SoundIo *m_soundio = nullptr;
    SoundIoDevice *m_device = nullptr;
    SoundIoInStream *m_inStream = nullptr;
    SoundIoRingBuffer *m_buffer = nullptr;

    volatile std::atomic_bool m_running;
};

#endif // __ANDROID__

#endif // AUDIO_BE_SOUNDIO_H
