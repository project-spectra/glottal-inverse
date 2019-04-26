#include "audio.h"


void destroyAudio(audio_s& audio)
{
    soundio_ring_buffer_destroy(audio.buffer);
    soundio_instream_destroy(audio.inStream);
    soundio_device_unref(audio.device);
    soundio_destroy(audio.soundio); 
}
