#include <iostream>
#include "audio.h"


bool startStream(audio_s& audio)
{
    int err = soundio_instream_start(audio.inStream);
    if (err != 0) {
        std::cerr << "soundio: unable to start input device: " << soundio_strerror(err) << std::endl;
        return true;
    }
   
    audio.running = true;

    return false;
}
