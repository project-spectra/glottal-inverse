#ifndef __ANDROID__

#include "audio.h"
#include "audio_be_soundio.h"


bool SoundIoAudioBackend::hasAtLeastOneWindow() {
    int fillCount = soundio_ring_buffer_fill_count(m_buffer);
    int windowCount = m_windowLength * m_inStream->bytes_per_frame;

    return (fillCount >= windowCount);
}

#endif
