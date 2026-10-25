# XPM image-data allocation size

`CreateXImage` now checks the XImage row stride against `SIZE_MAX` and performs
the row-count multiplication in `size_t` before allocating image data. This
avoids an intermediate signed-integer overflow while retaining all valid image
dimensions.

The focused regression is `tests/automated/check-xpm-create-image-size.py`.
