#ifndef __ANDROID__

#include "audio.h"
#include "audio_be_soundio.h"


void SoundIoAudioBackend::destroyAudio()
{
    soundio_ring_buffer_destroy(m_buffer);
    soundio_instream_destroy(m_inStream);
    soundio_device_unref(m_device);
    soundio_destroy(m_soundio);

    m_buffer = nullptr;
    m_inStream = nullptr;
    m_device = nullptr;
    m_soundio = nullptr;
    m_running.store(false);
}

SoundIoAudioBackend::~SoundIoAudioBackend()
{
    destroyAudio();
}

#endif
