#include <array>
#include <iostream>
#include "audio.h"
#include "audio_be_soundio.h"


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


bool SoundIoAudioBackend::initAudio(double targetWindowLength)
{
    // in s
    const double targetBufferLength(1.5 * targetWindowLength);

    m_soundio = soundio_create();
    if (m_soundio == nullptr) {
        std::cerr << "soundio: out of memory" << std::endl;
        return true;
    }

    int err = soundio_connect(m_soundio);
    if (err != 0) {
        std::cerr << "soundio: error connecting: " << soundio_strerror(err) << std::endl;
        return true;
    }

    soundio_flush_events(m_soundio);

    int device_index = soundio_default_input_device_index(m_soundio);
    m_device = soundio_get_input_device(m_soundio, device_index);
    if (m_device == nullptr) {
        std::cerr << "soundio: no input devices available" << std::endl;
        return true;
    }

    std::cout << "soundio: selected device: " << m_device->name << std::endl;
    
    if (m_device->probe_error != 0) {
        std::cerr << "soundio: unable to probe device: " << soundio_strerror(m_device->probe_error) << std::endl;
        return true;
    }
    
    soundio_device_sort_channel_layouts(m_device);

    auto layout = soundio_best_matching_channel_layout(
            soundio_channel_layout_get_default(1), 1,
            m_device->layouts, m_device->layout_count
    );
    if (layout == nullptr) {
        layout = &m_device->layouts[0];
    }

    int selectedSampleRate = 0;
    for (int sampleRate : prioSampleRates) {
        if (soundio_device_supports_sample_rate(m_device, sampleRate)) {
            selectedSampleRate = sampleRate;
            break;
        }
    }
    if (selectedSampleRate == 0) {
        selectedSampleRate = m_device->sample_rates[0].max;
    }
    
    SoundIoFormat selectedFmt = SoundIoFormatInvalid;
    for (auto fmt : prioFormats) {
        if (soundio_device_supports_format(m_device, fmt)) {
            selectedFmt = fmt;
            break;
        }
    }
    if (selectedFmt == SoundIoFormatInvalid) {
        std::cerr << "soundio: floating point format unsupported" << std::endl;
        return true;
    }

    m_inStream = soundio_instream_create(m_device);
    if (m_inStream == nullptr) {
        std::cerr << "soundio: out of memory" << std::endl;
        return true;
    }
    m_inStream->format = selectedFmt;
    m_inStream->sample_rate = selectedSampleRate;
    m_inStream->layout = *layout;
    m_inStream->read_callback = &readCallback;
    m_inStream->overflow_callback = &overflowCallback;
    m_inStream->userdata = this;

    err = soundio_instream_open(m_inStream);
    if (err != 0) {
        std::cerr << "soundio: unable to open input stream: " << soundio_strerror(err) << std::endl;
        return true;
    }

    std::cout << "soundio: " << m_inStream->layout.name << " "
              << SAMPLE_RATE << " Hz "
              << soundio_format_string(selectedFmt) << std::endl;

    if (targetWindowLength > targetBufferLength) {
        std::cerr << "soundio: window length is greater than buffer length" << std::endl;
        return true;
    }

    m_windowLength = targetWindowLength * m_inStream->sample_rate;

    const int capacity = targetBufferLength * m_inStream->sample_rate * m_inStream->bytes_per_frame;
    m_buffer = soundio_ring_buffer_create(m_soundio, capacity);
    if (m_buffer == nullptr) {
        std::cerr << "soundio: out of memory" << std::endl;
        return true;
    }

    return false;
}

