# Image-cache byte counter

The image conversion path that copies inverted image data can allocate a byte
buffer larger than `INT_MAX`. Its copy loop now uses `size_t`, matching the
checked byte count and preventing signed-counter wraparound on large images.

Normal image conversion and cleanup behavior are unchanged. The regression is
covered by `tests/automated/check-image-conversion-cleanup.py`.
