#include "audio_be_file.h"


const char *FileAudioBackend::getName() {
    return "fileio";
}

bool FileAudioBackend::isRunning() {
    return m_running;
}

int FileAudioBackend::getSampleRate() {
    return m_sampleRate;
}

int FileAudioBackend::getWindowLength() {
    return m_windowLength;
}
