#ifdef __ANDROID__

#include "audio_be_oboe.h"

bool OboeAudioBackend::startStream()
{
    oboe::Result result = m_stream->requestStart();
    if (result != oboe::Result::OK)
    {
        std::cerr << "oboe: unable to start input stream: " << oboe::convertToText(result) << std::endl;
        return true;
    }

    return false;
}


#endif // __ANDROID__
