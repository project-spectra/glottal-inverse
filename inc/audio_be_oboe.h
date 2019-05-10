#ifndef AUDIO_BE_OBOE_H
#define AUDIO_BE_OBOE_H

#ifdef __ANDROID__

#include <atomic>
#include <oboe/Oboe.h>

#include "audio.h"

class OboeAudioBackend : public AudioBackend
{
public:
    ~OboeAudioBackend();

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
    int m_windowLength;

    volatile std::atomic_bool m_running;
};

#endif // __ANDROID__

#endif // AUDIO_BE_OBOE_H
