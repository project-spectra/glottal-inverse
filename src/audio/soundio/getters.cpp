#include "audio_be_soundio.h"

const char *SoundIoAudioBackend::getName()
{
    return "soundio";
}

bool SoundIoAudioBackend::isRunning()
{
    return m_running.load();
}

int SoundIoAudioBackend::getSampleRate()
{
    return m_inStream->sample_rate;
}

int SoundIoAudioBackend::getWindowLength()
{
    return m_windowLength;
}
