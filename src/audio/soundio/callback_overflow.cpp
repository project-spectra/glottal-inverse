#ifndef __ANDROID__

#include <iostream>
#include "audio.h"
#include "audio_be_soundio.h"


void SoundIoAudioBackend::overflowCallback(struct SoundIoInStream *inStream)
{
    (void) inStream;

    static int count = 0;
    std::cerr << "soundio: overflow (#" << ++count << ")" << std::endl;
}

#endif
