#ifdef __ANDROID__

#include "audio_be_oboe.h"


bool OboeAudioBackend::hasAtLeastOneWindow()
{
    int fillCount = ringbuf_buffer_size(m_buffer);
    int windowCount = m_windowLength * m_stream->getBytesPerFrame();

    return (fillCount >= windowCount);
}


#endif // __ANDROID__
