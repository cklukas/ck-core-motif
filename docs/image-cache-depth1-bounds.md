# Depth-1 image-copy bounds

The depth-1 XYPixmap conversion path now rejects a checked image byte count
larger than Xt's `Cardinal` allocation range before copying image data. The
bounded count is converted explicitly for the allocator, avoiding truncation
on platforms where `size_t` is wider.

The focused source contract is checked by
`tests/automated/check-image-scaling-depth1.py`.
