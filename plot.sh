#!/bin/bash

sleep 2

script=$(mktemp)

plot_time() {
    echo | gnuplot <<EOFMarker
        set term wxt enh font "arial,12"
        set datafile sep ","

        set title "$1"

        set xrange [0:0.2]
        set yrange $3

        plot "$2" with lines

        bind all 'd' 'done = 1'
        done = 0
        while (!done) {
            plot "$2" with lines
            replot
        }
EOFMarker
}

plot_time 'Phase-difference graph' \
          'iaif_source.dat' \
          '[0:6.28]' &


wait
