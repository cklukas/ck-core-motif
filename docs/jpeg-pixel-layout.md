---
nav_exclude: true
---
# JPEG XImage pixel layout

JPEG conversion used fixed two-byte and four-byte writes based only on root
visual depth.  That overran depth-8 images, mishandled valid 24-bit XImages,
and ignored row padding.

The writer now derives the pixel width and byte order from the created
`XImage`, addresses each row through `bytes_per_line`, and rejects unsupported
pixel widths cleanly.  The existing visual-mask conversion remains intact.
`tests/automated/check-jpeg-pixel-layout.py` protects this contract.
