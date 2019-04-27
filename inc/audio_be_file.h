#ifndef AUDIO_BE_FILE_H
#define AUDIO_BE_FILE_H


#include <string>
#include <vector>

#include "audio.h"

class FileAudioBackend : public AudioBackend
{
public:
    FileAudioBackend(const char *filename);

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

    enum SupportedTypes { WAV, MP3, FLAC, UNKNOWN };

private:
    void stopIfFileEnd();

    std::string m_filename;
    SupportedTypes m_filetype;
    uint64_t m_length;

    std::vector<float> m_data;
    int m_dataIndex;

    int m_windowLength;
    int m_sampleRate;

    bool m_running;
};


#endif // AUDIO_BE_FILE_H
