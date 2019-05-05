#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <memory>

#include "audio.h"
#include "audio_be_soundio.h"
#include "audio_be_file.h"
#include "gcoi_sigma.h"
#include "gci_yaga.h"
#include "glottal.h"
#include "gnuplot.h"
#include "iaif.h"
#include "vtl.h"
#include "normalize.h"
#include "pitch.h"
#include "print_iter.h"

static std::shared_ptr<AudioBackend> audio;

int SAMPLE_RATE;
int WINDOW_LENGTH;


void sighand(int signum) {
    (void) signum;
    audio->stopStream();
}

int main(int argc, char *argv[]) {

    // Target window length of 60ms
    constexpr double targetWindowLength = 60. / 1000.;

    AudioBackend *backend;

    if (argc == 1) {
        backend = new SoundIoAudioBackend;
    } else {
        backend = new FileAudioBackend(argv[1]);
    }

    audio.reset(backend);

    if (audio->initAudio(targetWindowLength)) {
        std::cout << " **** Unable to init audio input ****" << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit([]() { audio->destroyAudio(); });
    std::signal(SIGINT, sighand);
    std::signal(SIGTERM, sighand);

    if (audio->startStream()) {
        std::cout << " **** Unable to start audio input ****" << std::endl;
        return EXIT_FAILURE;
    }

    SAMPLE_RATE = audio->getSampleRate();
    WINDOW_LENGTH = audio->getWindowLength();

    std::cout << " ==== Starting ====" << std::endl;

    valarray md(WINDOW_LENGTH);

    valarray g(WINDOW_LENGTH);
    valarray dg(WINDOW_LENGTH);

    double f0est, T0est;

    double meanVTL = -1;

    while (audio->isRunning()) {
        if (!audio->hasAtLeastOneWindow()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        // convert to doubles
        audio->loadWindow(md);

        if (!pitch_AMDF(md, &f0est, &T0est)) {
            //std::cout << "  (/)  No voiced speech detected" << std::endl;
            continue;
        }

        // estimate vocal tract length
        //double vtl = estimateVTL(md);
        double vtl = -1;

        meanVTL = meanVTL > 0 ? (meanVTL + vtl) / 2. : vtl;

        // estimate glottal source with IAIF
        computeIAIF(md, g, dg);

        // estimate GCIs
        std::vector<int> GCIs;
        std::vector<int> GOIs;

        gcoi_sigma(g, GCIs, GOIs);
        //gci_yaga(dg, T0est, GCIs, GOIs);
        
        printIterable(GCIs, "GCIs");
        printIterable(GOIs, "GOIs");

        // estimate Open quotient
        //double meanOq = estimateOq(GCIs, GOIs);
        double meanOq = -1;
               
        // print results
        std::cout << std::endl;
        std::cout << "  (*) Estimated:" << std::endl;
        std::cout << "      - f0: " << (int) f0est << " Hz" << std::endl;
        std::cout << "      - VTL: " << std::fixed << std::setprecision(1) << vtl << " cm" << std::endl;
        std::cout << "      - Oq: " << std::setprecision(2) << meanOq << std::endl;

        // write and plot
        //writePlotData(md, GNUPLOT_FILE_SPEECH);
        //writePlotData(g, GNUPLOT_FILE_SOURCE);
        //writePlotData(dg, GNUPLOT_FILE_SOURCE_DERIV);
    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    std::cout << std::endl;
    std::cout << "  (*) Final estimations:" << std::endl;
    std::cout << "      - Mean VTL: " << std::fixed << std::setprecision(1) << meanVTL << " cm " << std::endl;

    audio->destroyAudio();

    return EXIT_SUCCESS;
}
