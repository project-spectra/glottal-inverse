#include <cstring>
#include <iostream>
#include "audio.h"
#include "audio_be_soundio.h"


void SoundIoAudioBackend::readCallback(struct SoundIoInStream *inStream, int frameCountMin, int frameCountMax)
{
    auto self = static_cast<SoundIoAudioBackend *>(inStream->userdata);
    
    if (!self->m_running.load())
        return;
    
    struct SoundIoChannelArea *areas;
    int err;

    char *writePtr = soundio_ring_buffer_write_ptr(self->m_buffer);
    int freeBytes = soundio_ring_buffer_free_count(self->m_buffer);
    int freeCount = freeBytes / inStream->bytes_per_frame;

    if (freeCount < frameCountMin) {
        std::cerr << "soundio: ring buffer overflow" << std::endl;
        self->m_running.store(false);
        return;
    }

    int writeFrames = std::min(freeCount, frameCountMax);
    int framesLeft = writeFrames;

    while (true) {
        int frameCount = framesLeft;

        err = soundio_instream_begin_read(inStream, &areas, &frameCount);
        if (err != 0) {
            std::cerr << "soundio: begin read error: " << soundio_strerror(err) << std::endl;
            self->m_running.store(false);
            return;
        }
        
        if (frameCount == 0) {
            break;
        }

        if (areas == nullptr) {
            // overflow, fill the ring buffer with silence.
            memset(writePtr, 0, frameCount * inStream->bytes_per_frame);
        } else {
            for (int frame = 0; frame < frameCount; ++frame) {
                for (int ch = 0; ch < inStream->layout.channel_count; ++ch) {
                    memcpy(writePtr, areas[ch].ptr, inStream->bytes_per_sample);
                    areas[ch].ptr += areas[ch].step;
                    writePtr += inStream->bytes_per_sample;
                }    
            }
        }

        err = soundio_instream_end_read(inStream);
        if (err != 0) {
            std::cerr << "soundio: end read error: " << soundio_strerror(err) << std::endl;
            self->m_running.store(false);
            return;
        }

        framesLeft -= frameCount;
        if (framesLeft <= 0)
            break;
    }

    int advanceBytes = writeFrames * inStream->bytes_per_frame;
    soundio_ring_buffer_advance_write_ptr(self->m_buffer, advanceBytes);
}
