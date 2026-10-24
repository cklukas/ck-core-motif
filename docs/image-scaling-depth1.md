# Depth-1 image-copy bounds

The depth-1 XYPixmap conversion copied `height * bytes_per_line` through an
`int`, then allocated that result.  Large images could wrap the product, and a
failed allocation could replace a cached image's data pointer with `NULL`.

The conversion now checks the product in `size_t`, allocates into a temporary
pointer, and updates the image only after success.  Lock and non-cached cleanup
remain balanced on rejection.  `tests/automated/check-image-scaling-depth1.py`
protects these invariants.
