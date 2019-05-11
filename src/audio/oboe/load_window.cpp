#ifdef __ANDROID__

#include "audio_be_oboe.h"


void OboeAudioBackend::loadWindow(valarray& md)
{
    oboe::AudioFormat fmt = m_stream->getFormat();
    int numChannels = m_stream->getChannelCount();

    int readLen = m_windowLength * m_stream->getBytesPerFrame();
    void *targetBuf = malloc(readLen);

    ringbuf_memcpy_from(targetBuf, m_buffer, readLen);

    if (fmt == oboe::AudioFormat::I16) {
        // convert to float
        float *floatBuf = (float *) malloc(m_windowLength * numChannels * sizeof(float));

        oboe::convertPcm16ToFloat((int16_t *) targetBuf, floatBuf, m_windowLength * numChannels);

        free(targetBuf);
        targetBuf = floatBuf;
    }

    md.resize(m_windowLength);
    for (int i = 0; i < m_windowLength; ++i) {
        md[i] = static_cast<double>(((float *) targetBuf)[i * numChannels]);
    }

    free(targetBuf);
}


#endif // __ANDROID__
