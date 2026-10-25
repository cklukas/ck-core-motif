---
nav_exclude: true
---
# Mwm bitmap path bounds

The window manager combines the configured bitmap directory with a relative
icon name before asking Motif to load the image.  Both strings are resource or
application input, so adding their lengths directly and then using `sprintf`
could wrap the allocation arithmetic or overrun the result.

`GetNamedPixmap` now measures the combined path with `snprintf`, allocates the
exact result plus its terminator, and performs a bounded second formatting
pass.  Bitmap lookup order and names are unchanged.

`tests/automated/check-mwm-image-path-bounds.py` protects this contract.
