#ifndef __ANDROID__

#include <iostream>
#include "audio.h"
#include "audio_be_soundio.h"


bool SoundIoAudioBackend::startStream()
{
    int err = soundio_instream_start(m_inStream);
    if (err != 0) {
        std::cerr << "soundio: unable to start input device: " << soundio_strerror(err) << std::endl;
        return true;
    }
   
    m_running.store(true);

    return false;
}


void SoundIoAudioBackend::stopStream()
{
    int err = soundio_instream_pause(m_inStream, true);
    if (err != 0) {
        std::cerr << "soundio: unable to stop input device: " << soundio_strerror(err) << std::endl;
    }

    // Stop running either way.
    m_running.store(false);
}

#endif
