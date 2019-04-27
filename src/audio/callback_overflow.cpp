#include <iostream>
#include "audio.h"


void overflowCallback(struct SoundIoInStream *inStream)
{
    (void) inStream;

    static int count = 0;
    std::cerr << "soundio: overflow (#" << ++count << ")" << std::endl;
}
