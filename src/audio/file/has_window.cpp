#include "audio_be_file.h"


bool FileAudioBackend::hasAtLeastOneWindow()
{
    return m_dataIndex + m_windowLength < m_data.size();
}
