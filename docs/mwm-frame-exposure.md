# MWM frame exposure drawables

X exposure events may be delivered while MWM is destroying a client frame.
Both frame and base-window exposure callbacks now require a realized drawable
before painting bevel rectangles, keeping late events harmless.
