#include <climits>
#include <cstring>
#include "audio.h"
#include "audio_be_soundio.h"


template<typename T>
static void fillArray(T arr[], int len, int numChannels, bool isForeign, valarray& md);

template<typename T>
static void swapEndian(T arr[], int len);


void SoundIoAudioBackend::loadWindow(valarray& md)
{
    // again, assume it's mono channel
    // also assume that the buffer has enough frames
    char *readPtr = soundio_ring_buffer_read_ptr(m_buffer);

    SoundIoFormat fmt = m_inStream->format;
    int numChannels = m_inStream->layout.channel_count;

    bool formatIsDouble = (fmt == SoundIoFormatFloat64NE
                                || fmt == SoundIoFormatFloat64FE);
    
    bool formatIsForeign = (fmt == SoundIoFormatFloat64FE
                                || fmt == SoundIoFormatFloat32FE);

    size_t readLen = m_windowLength * m_inStream->bytes_per_frame;
    void *targetBuf = malloc(readLen);

    memcpy(targetBuf, readPtr, readLen);

    if (formatIsDouble) {
        fillArray((double *) targetBuf, m_windowLength, numChannels, formatIsForeign, md);
    } else {
        fillArray((float *) targetBuf, m_windowLength, numChannels, formatIsForeign, md);
    }

    free(targetBuf);
    soundio_ring_buffer_advance_read_ptr(m_buffer, readLen);
}

template<typename T>
static void fillArray(T arr[], int len, int numChannels, bool isForeign, valarray& md)
{
    if (isForeign) {
        swapEndian(arr, len);
    }

    md.resize(len);
    for (int i = 0; i < len; ++i) {
        md[i] = static_cast<double>(arr[i * numChannels]);
    }

}

template<typename T>
static void swapEndian(T arr[], int len)
{
    static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    for (int i = 0; i < len; ++i) {
        source.u = arr[i];

        for (size_t k = 0; k < sizeof(T); ++k) {
            dest.u8[k] = source.u8[sizeof(T) - k - 1];
        }

        arr[i] = dest.u;
    }
}


