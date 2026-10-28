# Converted image cleanup ownership

Depth-one `XYPixmap` images may be temporarily converted by replacing their
source data with a buffer allocated by `XtMalloc`. The original source data
still belongs to the image loader, while the replacement belongs to Xt.

The conversion cleanup path now records whether the current image data is the
Xt-owned replacement. It releases that buffer with `XtFree` before invoking
`XDestroyImage`, while leaving untouched loader-owned data for the XImage
destructor. The original buffer is released separately after the wrapper is
destroyed.

The ownership contract is covered by
`tests/automated/check-image-converted-cleanup.py`.
