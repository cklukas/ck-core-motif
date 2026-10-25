# DragUnder region geometry bounds

DragUnder now skips single-region highlight and shadow drawing when the region
is no larger than twice its border offset. Width and height subtraction is
performed after signed bounds checks, preventing unsigned wraparound from
creating oversized X draw requests for collapsed drop sites.

The focused regression is `tests/automated/check-dragunder-geometry-bounds.py`.
