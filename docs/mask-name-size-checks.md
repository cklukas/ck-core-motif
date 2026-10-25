---
nav_exclude: true
---
# Image mask-name size checks

Image mask names reserve three extra bytes for the generated prefix and
terminator. The cache now checks the `strlen` addition before allocating that
buffer, both for embedded XPM masks and `XmeGetMask`. An unrepresentable size
returns the existing failure result and releases any temporary mask image.

The focused regression is `tests/automated/check-mask-name-size.py`.
