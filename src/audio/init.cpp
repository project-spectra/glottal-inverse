#include <array>
#include <iostream>
#include "audio.h"


int SAMPLE_RATE;
int AUDIO_FORMAT;
int WINDOW_LENGTH;



static const std::array<SoundIoFormat, 5> prioFormats = {
    SoundIoFormatFloat64NE,
    SoundIoFormatFloat64FE,
    SoundIoFormatFloat32NE,
    SoundIoFormatFloat32FE,
    SoundIoFormatInvalid,
};

static const std::array<int, 5> prioSampleRates = {
    48000,
    44100,
    96000,
    24000,
    0,
};

// in s
static const double targetWindowLength = 25. / 1000.;
static const double targetBufferLength = 300. / 1000.;


bool initAudio(audio_s& audio)
{
    audio.soundio = soundio_create();
    if (audio.soundio == nullptr) {
        std::cerr << "soundio: out of memory" << std::endl;
        return true;
    }

    int err = soundio_connect(audio.soundio);
    if (err != 0) {
        std::cerr << "soundio: error connecting: " << soundio_strerror(err) << std::endl;
        return true;
    }

    soundio_flush_events(audio.soundio);

    int device_index = soundio_default_input_device_index(audio.soundio);
    audio.device = soundio_get_input_device(audio.soundio, device_index);
    if (audio.device == nullptr) {
        std::cerr << "soundio: no input devices available" << std::endl;
        return true;
    }

    std::cout << "soundio: selected device: " << audio.device->name << std::endl;
    
    if (audio.device->probe_error != 0) {
        std::cerr << "soundio: unable to probe device: " << soundio_strerror(audio.device->probe_error) << std::endl;
        return true;
    }

    auto layout = soundio_best_matching_channel_layout(
            soundio_channel_layout_get_default(1), 1,
            audio.device->layouts, audio.device->layout_count
    );
    if (layout == nullptr) {
        std::cerr << "soundio: unable to find mono channel input" << std::endl;
        return true;
    }

    SAMPLE_RATE = 0;
    for (int sampleRate : prioSampleRates) {
        if (soundio_device_supports_sample_rate(audio.device, sampleRate)) {
            SAMPLE_RATE = sampleRate;
            break;
        }
    }
    if (SAMPLE_RATE == 0) {
        SAMPLE_RATE = audio.device->sample_rates[0].max;
    }
    
    AUDIO_FORMAT = SoundIoFormatInvalid;
    for (auto fmt : prioFormats) {
        if (soundio_device_supports_format(audio.device, fmt)) {
            AUDIO_FORMAT = fmt;
            break;
        }
    }
    if (AUDIO_FORMAT == SoundIoFormatInvalid) {
        std::cerr << "soundio: floating point format unsupported" << std::endl;
        return true;
    }

    audio.inStream = soundio_instream_create(audio.device);
    if (audio.inStream == nullptr) {
        std::cerr << "soundio: out of memory" << std::endl;
        return true;
    }
    audio.inStream->format = static_cast<enum SoundIoFormat>(AUDIO_FORMAT);
    audio.inStream->sample_rate = SAMPLE_RATE;
    audio.inStream->layout = *soundio_channel_layout_get_default(1); layout;
    audio.inStream->read_callback = &readCallback;
    audio.inStream->overflow_callback = &overflowCallback;
    audio.inStream->userdata = &audio;

    err = soundio_instream_open(audio.inStream);
    if (err != 0) {
        std::cerr << "soundio: unable to open input stream: " << soundio_strerror(err) << std::endl;
        return true;
    }

    std::cout << "soundio: " << audio.inStream->layout.name << " "
              << SAMPLE_RATE << " Hz "
              << soundio_format_string(static_cast<enum SoundIoFormat>(AUDIO_FORMAT)) << std::endl;

    if (targetWindowLength > targetBufferLength) {
        std::cerr << "soundio: window length is greater than buffer length" << std::endl;
        return true;
    }

    WINDOW_LENGTH = targetWindowLength * audio.inStream->sample_rate;

    const int capacity = targetBufferLength * audio.inStream->sample_rate * audio.inStream->bytes_per_frame;
    audio.buffer = soundio_ring_buffer_create(audio.soundio, capacity);
    if (audio.buffer == nullptr) {
        std::cerr << "soundio: out of memory" << std::endl;
        return true;
    }

    return false;
}

