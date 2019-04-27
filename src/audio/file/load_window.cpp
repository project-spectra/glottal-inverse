#include "audio_be_file.h"


void FileAudioBackend::loadWindow(valarray& md)
{
    if (!m_running)
        return;

    // load window from dataIndex of length windowLengthh
    md.resize(m_windowLength);

    for (int i = 0; i < m_windowLength; ++i) {
        md[i] = m_data[m_dataIndex + i];
    }

    // update the index and check for file end
    m_dataIndex += m_windowLength;

    stopIfFileEnd();
}

void FileAudioBackend::stopIfFileEnd()
{
    if (m_dataIndex + m_windowLength >= m_data.size())
    {
        m_running = false;
    }
}
