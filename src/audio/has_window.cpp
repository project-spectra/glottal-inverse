#include "audio.h"


bool hasAtLeastOneWindow(audio_s& audio) {
    int fillCount = soundio_ring_buffer_fill_count(audio.buffer);
    int windowCount = WINDOW_LENGTH * audio.inStream->bytes_per_frame;

    return (fillCount >= windowCount);
}
