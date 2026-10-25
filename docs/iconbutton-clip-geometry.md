# IconButton text clipping geometry

IconButton clips label text against the available width and height after its
spacing margins.  Those bounds now subtract spacing in signed integers before
comparing with the text rectangle, so a narrow button cannot turn a negative
available size into a large wrapped clipping limit.
