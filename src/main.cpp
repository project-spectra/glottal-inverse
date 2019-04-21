#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <experimental/iterator>

#include <portaudio.h>

#include "audio_in.h"
#include "pitch.h"
#include "iaif.h"
#include "normalize.h"
#include "gci_sedreams.h"
#include "glottal.h"
#include "gnuplot.h"
#include "gsl_util.h"
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

    static_vector(md);

    static_vector(g);
    static_vector(dg);

    double f0est, T0est;

    while (!stop) {
        // record a window
        data->frameIndex = 0;
        recordWindow(stream);

        // convert to doubles
        for (size_t i = 0; i < WINDOW_LENGTH; ++i) {
            gsl_vector_set(md, i, data->recordedSamples[i]);
        }
        
        if (!pitch_AMDF(md, &f0est, &T0est)) {
            //std::cout << "  (/)  No voiced speech detected" << std::endl;
            //continue;
        }
        
        // estimate glottal source with IAIF
        normalize(md, 1.);
        computeIAIF(g, dg, md);
        normalize(g, 1.);
        normalize(dg, 1.);

        // estimate GCIs
        //auto gci = gci_sedreams(md, SAMPLE_RATE, f0est);

        // estimate glottal parameters
        //auto vp = estimateGlottal(g, dg, SAMPLE_RATE, gci);
               
        // print results
        //std::cout << "  (*) Estimated:" << std::endl
        //          << "      - f0: " << (int) f0est << " Hz" << std::endl;

        // write and plot
        writePlotData(md, GNUPLOT_FILE_SPEECH);
        writePlotData(g, GNUPLOT_FILE_IAIF_SOURCE);
        writePlotData(dg, GNUPLOT_FILE_IAIF_SOURCE_DERIV);

    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    free(data);

    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}
