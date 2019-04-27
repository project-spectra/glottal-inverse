#if __cplusplus < 201703L
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

#include <iostream>

#include "audio_be_file.h"

FileAudioBackend::FileAudioBackend(const char *filename)
    : m_filename(filename), m_running(false)
{
    
    fs::path filepath(m_filename);

    std::string ext(filepath.has_extension() ? filepath.extension() : "");
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

    if (ext.compare(".WAV") == 0) {
        m_filetype = WAV;
    } else if (ext.compare(".MP3") == 0) {
        m_filetype = MP3;
    } else if (ext.compare(".FLAC") == 0) {
        m_filetype = FLAC;
    } else {
        m_filetype = UNKNOWN;
    }

}
