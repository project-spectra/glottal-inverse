#!/bin/bash

sleep 2

script=$(mktemp)

plot_time() {
    echo | gnuplot <<EOFMarker
        set term wxt enh font "arial,12"
        set datafile sep ","

        set title "$1"

        set xrange [0:0.2]
        set yrange [0:5]

        plot "$2" with lines

        bind all 'd' 'done = 1'
        done = 0
        while (!done) {
            plot "$2" with lines
            replot
        }
EOFMarker
}

plot_time 'IAIF glottal flow' \
          'iaif_source.dat' &

plot_time 'Glottal flow derivative' \
          'iaif_source_deriv.dat' &

wait
