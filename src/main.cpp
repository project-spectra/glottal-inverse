#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "constants.h"
#include "audio.h"
#include "pitch.h"
#include "iaif.h"
#include "normalize.h"
#include "gci_yaga.h"
#include "gnuplot.h"
#include "print_iter.h"


static audio_s audio;

void sighand(int signum) {
    (void) signum;
    audio.running = false;
}

int main() {

    audio.soundio = nullptr;
    audio.device = nullptr;
    audio.inStream = nullptr;
    audio.buffer = nullptr;
    
    if (initAudio(audio)) {
        std::cout << " **** Unable to init audio input ****" << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit([]() { destroyAudio(audio); });
    std::signal(SIGINT, sighand);
    std::signal(SIGTERM, sighand);

    if (startStream(audio)) {
        std::cout << " **** Unable to start audio input ****" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << " ==== Recording ====" << std::endl;

    valarray md(WINDOW_LENGTH);

    valarray g(WINDOW_LENGTH);
    valarray dg(WINDOW_LENGTH);

    double f0est, T0est;

    while (audio.running) {
        if (!hasAtLeastOneWindow(audio)) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        // convert to doubles
        loadWindow(audio, md);
        
        if (!pitch_AMDF(md, &f0est, &T0est)) {
            //std::cout << "  (/)  No voiced speech detected" << std::endl;
            continue;
        }

        // estimate glottal source with IAIF
        normalize(md);
        computeIAIF(md, g, dg);
        normalize(g);
        normalize(dg);

        // estimate GCIs
        //gci_yaga(dg);
               
        // print results
        std::cout << "  (*) Estimated:" << std::endl
                  << "      - f0: " << (int) f0est << " Hz" << std::endl;

        // write and plot
        writePlotData(md, GNUPLOT_FILE_SPEECH);
        writePlotData(g, GNUPLOT_FILE_SOURCE);
        writePlotData(dg, GNUPLOT_FILE_SOURCE_DERIV);
    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    return EXIT_SUCCESS;
}
