---
nav_exclude: true
---

# Image-cache color callback inputs

The XPM image-cache callbacks now reject null displays, color structures,
screens, and pixel arrays (as well as non-positive pixel counts) before cache
or X11 access.  This keeps malformed image metadata and teardown-time cleanup
from dereferencing incomplete callback state.

The regression check is
`tests/automated/check-image-cache-color-inputs.py`.
