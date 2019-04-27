#include "audio_be_file.h"


bool FileAudioBackend::startStream()
{
    m_dataIndex = 0;
    m_running = true;

    return false;
}

void FileAudioBackend::stopStream()
{
    m_running = false;
}
