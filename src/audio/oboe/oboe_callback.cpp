#ifdef __ANDROID__

#include "audio_be_oboe.h"


oboe::DataCallbackResult OboeAudioBackend::onAudioReady(oboe::AudioStream *stream, void *data, int32_t numFrames)
{
    int writeCount = numFrames * stream->getBytesPerFrame();
    int freeBytes = ringbuf_bytes_free(m_buffer);

    if (freeBytes < writeCount) {
        std::cerr << "oboe: ring buffer overflow" << std::endl;
        return oboe::DataCallbackResult::Stop;
    }

    ringbuf_memcpy_into(m_buffer, data, numFrames * stream->getBytesPerFrame());

    return oboe::DataCallbackResult::Continue;
}


#endif // __ANDROID__
