#ifdef __ANDROID__

#include "audio_be_oboe.h"


bool OboeAudioBackend::initAudio(double windowLength)
{
    double bufferLength = 1.5 * windowLength;

    oboe::AudioStreamBuilder builder;
    setStreamParameters(&builder);

    oboe::Result result = builder.openStream(&m_stream);
    if (result != oboe::Result::OK || m_stream == nullptr)
    {
        std::cerr << "oboe: unable to open input stream: " << oboe::convertToText(result) << std::endl;
        return true;
    }

    m_windowLength = windowLength * m_stream->getSampleRate();

    m_buffer = ringbuf_new(m_stream->getBytesPerFrame() * m_stream->getSampleRate() * bufferLength);
    if (m_buffer == nullptr)
    {
        std::cerr << "oboe: unable to create circular buffer" << std::endl;
        return true;
    }

    return false;
}

void OboeAudioBackend::setStreamParameters(oboe::AudioStreamBuilder *builder)
{
    builder->setCallback(this)
        ->setDirection(oboe::Direction::Input)
        ->setChannelCount(oboe::ChannelCount::Mono)
        ->setFormat(oboe::AudioFormat::Float)
        ->setSharingMode(oboe::SharingMode::Exclusive)
        ->setPerformanceMode(oboe::PerformanceMode::PowerSaving);
}


#endif // __ANDROID__
