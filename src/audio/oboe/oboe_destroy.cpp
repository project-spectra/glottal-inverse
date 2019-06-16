#ifdef __ANDROID__


#include "audio_be_oboe.h"


void OboeAudioBackend::destroyAudio()
{
    oboe::Result result = m_stream->close();
    if (result != oboe::Result::OK)
    {
        std::cerr << "oboe: unable to close input stream: " << oboe::convertToText(result) << std::endl;
    }

    ringbuf_free(&m_buffer);
}


#endif // __ANDROID__
