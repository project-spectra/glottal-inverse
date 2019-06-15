#include <iostream>
#include "audio_be_file.h"

#include "dr_wav.h"
#include "dr_mp3.h"
#include "dr_flac.h"


bool FileAudioBackend::initAudio(double targetLength)
{
    unsigned int numChannels;
    unsigned int sampleRate;
    uint64_t totalPCMFrameCount;

    float *data(nullptr);

    const char *filename = m_filename.c_str();

    if (m_filetype == WAV)
    {
        data = drwav_open_file_and_read_pcm_frames_f32(filename, &numChannels, &sampleRate, &totalPCMFrameCount);
        if (data == nullptr) {
            std::cerr << "fileio: unable to decode WAV file" << std::endl;
            return true;
        }
    }
    else if (m_filetype == MP3)
    {
        drmp3_config config;
        data = drmp3_open_file_and_read_f32(filename, &config, &totalPCMFrameCount);
        if (data == nullptr) {
            std::cerr << "fileio: unable to decode MP3 file" << std::endl;
            return true;
        }

        numChannels = config.outputChannels;
        sampleRate = config.outputSampleRate;
    }
    else if (m_filetype == FLAC)
    {
        data = drflac_open_file_and_read_pcm_frames_f32(filename, &numChannels, &sampleRate, &totalPCMFrameCount);
        if (data == nullptr) {
            std::cerr << "fileio: unable to decode FLAC file" << std::endl;
            return true;
        }
    }
    else 
    {
        std::cerr << "fileio: unsupported audio file" << std::endl;
        return true;
    }

    // now copy the data into m_data
    m_data.resize(totalPCMFrameCount, 0.);
    for (int i = 0; i < totalPCMFrameCount; ++i) {
        m_data[i] = data[i * numChannels]; // get mono channel data
    }

    // Free the buffer
    switch (m_filetype) {
    case WAV:
        drwav_free(data);
        break;
    case MP3:
        drmp3_free(data);
        break;
    case FLAC:
        drflac_free(data);
        break;
    default:
        std::cerr << "fileio: critical error" << std::endl;
        return true;
    }

    m_sampleRate = sampleRate;
    m_windowLength = targetLength * sampleRate;

    if (totalPCMFrameCount < m_windowLength)
    {
        std::cerr << "fileio: audio file is too short (must be at least " << targetLength * 1000 << " ms long)" << std::endl;
        return true;
    }

    // Print summary of file characteristics
    std::cout << "fileio: reading \"" << filename << "\" " << sampleRate << " Hz f.32" << std::endl;

    return false;
}
