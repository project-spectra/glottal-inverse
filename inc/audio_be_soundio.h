#ifndef AUDIO_BE_SOUNDIO_H
#define AUDIO_BE_SOUNDIO_H


#include <atomic>
#include <soundio/soundio.h>

#include "audio.h"

class SoundIoAudioBackend : public AudioBackend
{
public:
    ~SoundIoAudioBackend();

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

    SoundIo *m_soundio;
    SoundIoDevice *m_device;
    SoundIoInStream *m_inStream;
    SoundIoRingBuffer *m_buffer;

    volatile std::atomic_bool m_running;
};


#endif // AUDIO_BE_SOUNDIO_H
