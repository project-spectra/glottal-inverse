#!/usr/bin/env python3

import time
import matplotlib.pyplot as plt
import numpy as np

TIME_RANGE = (0, .0625), (-1, 1)

#TIME_PLOT = ('Speech signal', 'speech.dat')
#TIME_PLOT = ('Glottal source', 'glottal_source_deriv.dat')
#TIME_PLOT = ('Glottal flow', 'glottal_source.dat')
TIME_PLOT = ('Group delay function', 'yaga.dat')


done = False
def press(event):
    global done
    if event.key.lower() in { 'd', 'q', 'escape' }:
        print("Done.")
        done = True

plt.ion()

fig, ax = plt.subplots()
fig.patch.set_facecolor('white')
ax.set_facecolor('white')

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
            points = ax.plot(t, y, linewidth=0.75, color='orange')[0]
        else:
            points.set_data(t, y)

        ax.draw_artist(points)
    except:
        pass

    fig.canvas.blit(ax.bbox)
    fig.canvas.draw_idle()

    return points

points = load_file()

fig.canvas.mpl_connect('key_press_event', press)
while not done:
    load_file(points)
    fig.canvas.start_event_loop(0.001)



