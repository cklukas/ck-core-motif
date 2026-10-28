# Scaled image row-stride validation

`_XmPutScaledImage` accepts an `XImage` supplied by a caller.  Checking the
total `height * bytes_per_line` allocation is not enough: a malformed image
can advertise a row stride smaller than the bytes required by its width and
pixel depth.  Scaling such an image could read past each row while copying
pixels.

The scaler now rejects non-positive `bits_per_pixel` values and verifies that
every row is at least `ceil(width * bits_per_pixel / 8)` bytes before it reads
the image.  Valid XImages retain the existing scaling and XPutImage paths.

The source contract is covered by
`tests/automated/check-image-scale-row-stride.py`.
