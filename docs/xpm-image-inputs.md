---
nav_exclude: true
---

# XPM raw-image conversion inputs

`XpmCreateImageFromXpmImage` now validates its display, source image, and
requested output pointers before reading image fields or allocating color
buffers.  Null or output-less calls return `XpmFileInvalid`, matching the
other public XPM conversion entry points and preserving initialized null
outputs.

Regression: `tests/automated/check-xpm-image-inputs.py`
