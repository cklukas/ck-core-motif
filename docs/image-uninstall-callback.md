---
nav_exclude: true
---

# Image uninstall callback inputs

The image-cache uninstall hash callback now returns safely when the hash value
or requested image pointer is null, before dereferencing the cached image.
Valid uninstall operations retain their existing removal behavior.

The regression check is `tests/automated/check-image-uninstall-callback.py`.
