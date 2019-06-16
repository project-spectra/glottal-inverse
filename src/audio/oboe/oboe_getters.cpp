#ifdef __ANDROID__

#include "audio_be_oboe.h"


const char *OboeAudioBackend::getName() {
    return "oboe";
}

bool OboeAudioBackend::isRunning() {
    return m_stream->getState() != oboe::StreamState::Started;
}

int OboeAudioBackend::getSampleRate() {
    return m_stream->getSampleRate();
}

int OboeAudioBackend::getWindowLength() {
    return m_windowLength;
}


#endif // __ANDROID__
