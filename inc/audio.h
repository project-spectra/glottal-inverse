#ifndef AUDIO_IN_H
#define AUDIO_IN_H


#include <valarray>

using valarray = std::valarray<double>;

extern int SAMPLE_RATE;
extern int WINDOW_LENGTH;

class AudioBackend
{
public:
    virtual const char *getName() = 0;
    virtual bool isRunning() = 0;

    virtual int getSampleRate() = 0;
    virtual int getWindowLength() = 0;

    virtual bool hasAtLeastOneWindow() = 0;
    virtual void loadWindow(valarray& md) = 0;

    virtual bool initAudio(double targetLength) = 0;
    virtual void destroyAudio() = 0;

    virtual bool startStream() = 0;
    virtual void stopStream() = 0;

};


#endif // AUDIO_IN_H
