#include <iostream>
#include <cstdlib>
#include <csignal>

#include <portaudio.h>

#include "audio_in.h"

volatile sig_atomic_t stop;

void terminate();
void inthand(int signum) {
    (void) signum;
    stop = 1;
}

int main() {
    PaError err;
    PaStream *stream;
    window_data data;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init failure: " << Pa_GetErrorText(err) << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit(terminate);

    if (!openStream(&stream, &data)) {
        std::cerr << "Exiting with error..." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << " ==== Recording ====" << std::endl;

    signal(SIGINT, inthand);

    while (!stop) {
        recordWindow(stream);
    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}
