#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <portaudio.h>

#include "constants.h"
#include "audio_in.h"
#include "pitch.h"
#include "iaif.h"
#include "normalize.h"
#include "gci_yaga.h"
#include "gnuplot.h"
#include "print_iter.h"


volatile std::atomic<bool> stop;

void terminate();
void inthand(int signum) {
    (void) signum;
    stop = true;
}

int main() {

    PaError err;
    PaStream *stream;
    window_data *data;
  
    data = static_cast<window_data *>(malloc(
        sizeof(window_data) + WINDOW_LENGTH * sizeof(sample)
    ));

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init failure: " << Pa_GetErrorText(err) << std::endl;
        free(data);
        return EXIT_FAILURE;
    }

    std::atexit(terminate);

    if (!openStream(&stream, data)) {
        std::cerr << "Exiting with error..." << std::endl;
        free(data);
        return EXIT_FAILURE;
    }
    
    std::cout << " ==== Recording ====" << std::endl;

    valarray md(WINDOW_LENGTH);

    valarray g(WINDOW_LENGTH);
    valarray dg(WINDOW_LENGTH);

    double f0est, T0est;

    while (!stop) {
        // record a window
        data->frameIndex = 0;
        recordWindow(stream);

        // convert to doubles
        for (size_t i = 0; i < WINDOW_LENGTH; ++i) {
            md[i] = data->recordedSamples[i];
        }
        
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
        gci_yaga(dg);
               
        // print results
        std::cout << "  (*) Estimated:" << std::endl
                  << "      - f0: " << (int) f0est << " Hz" << std::endl;

        // write and plot
        writePlotData(md, GNUPLOT_FILE_SPEECH);
        writePlotData(g, GNUPLOT_FILE_SOURCE);
        writePlotData(dg, GNUPLOT_FILE_SOURCE_DERIV);

    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    free(data);

    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}
