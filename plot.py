#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

TIME_RANGE = (0, .08), (-1, 1)
#TIME_PLOT = ('Speech signal', 'speech.dat')
#TIME_PLOT = ('Glottal source', 'iaif_source_deriv.dat')
TIME_PLOT = ('Glottal flow', 'iaif_source.dat')


fig, ax = plt.subplots()

plt.title(TIME_PLOT[0])

plt.xlabel('Time (s)')
plt.xlim(TIME_RANGE[0])
plt.xticks(np.linspace(*TIME_RANGE[0], 5))

plt.ylabel('Rel. amp.')
plt.ylim(TIME_RANGE[1])
plt.yticks(np.linspace(*TIME_RANGE[1], 5))

# Get background
plt.show(False)
plt.draw()

background = fig.canvas.copy_from_bbox(ax.bbox)

def load_file(points=None):
    fig.canvas.restore_region(background)

    try:
        t, y = np.genfromtxt(TIME_PLOT[1], delimiter=',').T
        if points is None:
            points = ax.plot(t, y, linewidth=1, color='orange')[0]
        else:
            points.set_data(t, y)

        ax.draw_artist(points)
    except:
        pass

    fig.canvas.blit(ax.bbox)

    return points

points = load_file()
while True:
    load_file(points)
    plt.pause(0.1)



