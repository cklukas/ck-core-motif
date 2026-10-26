# Image scaling conversions

Image-cache scaling now keeps table indices unsigned until converting to the
signed coordinates required by X11. The 8-bit fast path explicitly narrows a
pixel to the byte value consumed by `memset`, and cache allocation byte counts
are checked before conversion to Xt's `Cardinal` size.

The focused source contract is checked by
`tests/automated/check-image-scaling-conversions.py`.
