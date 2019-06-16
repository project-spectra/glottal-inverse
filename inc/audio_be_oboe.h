#ifndef AUDIO_BE_OBOE_H
#define AUDIO_BE_OBOE_H

#ifdef __ANDROID__

#include <atomic>
#include <iostream>

#include <oboe/Oboe.h>

#include "audio.h"
#include "ringbuf.h"

class OboeAudioBackend : public AudioBackend, public oboe::AudioStreamCallback 
{
public:
    explicit OboeAudioBackend();
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

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *autoData, int32_t numFrames);

private:
    void setStreamParameters(oboe::AudioStreamBuilder *builder);
    
    int m_windowLength;

    oboe::AudioStream *m_stream = nullptr;
    ringbuf_t m_buffer = nullptr;

    volatile std::atomic_bool m_running;
};

#endif // __ANDROID__

#endif // AUDIO_BE_OBOE_H
