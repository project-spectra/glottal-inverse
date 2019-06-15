#include <algorithm>
#include <iostream>

#include "audio_be_file.h"

static bool endsWith(const std::string &filename, const std::string &ext)
{
  return ext.length() <= filename.length() &&
         std::equal(ext.rbegin(), ext.rend(), filename.rbegin());
}

FileAudioBackend::FileAudioBackend(const char *filename)
    : m_filename(filename), m_running(false)
{
    std::string upFname(filename);
    std::transform(upFname.begin(), upFname.end(), upFname.begin(), ::toupper);

    if (endsWith(upFname, ".WAV")
            || endsWith(upFname, ".EGG")) {
        m_filetype = WAV;
    } else if (endsWith(upFname, ".MP3")) {
        m_filetype = MP3;
    } else if (endsWith(upFname, ".FLAC")) {
        m_filetype = FLAC;
    } else {
        m_filetype = UNKNOWN;
    }

}
