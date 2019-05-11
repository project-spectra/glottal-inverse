#ifdef __ANDROID__

#include "audio_be_oboe.h"

void OboeAudioBackend::stopStream()
{
    oboe::Result result = m_stream->stop(0L);
    if (result != oboe::Result::OK)
    {
        std::cerr << "oboe: unable to stop input stream: " << oboe::convertToText(result) << std::endl;
    }
}


#endif // __ANDROID__
