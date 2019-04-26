#include <iostream>
#include "audio.h"


bool pauseStream(audio_s& audio, bool pause)
{
    int err = soundio_instream_pause(audio.inStream, pause);
    if (err != 0) {
        std::cerr << "soundio: unable to " << (pause ? "pause" : "resume")
                  << " input device: " << soundio_strerror(err) << std::endl;
        return true;
    }

    return false;
}
