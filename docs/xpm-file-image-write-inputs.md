---
nav_exclude: true
---

# Validate XPM image-to-file inputs

`XpmWriteFileFromImage` now rejects a null display or a call with neither an
image nor a shape image before entering X11 image conversion.  Shape-only
exports remain supported, and the null-filename stdout behavior is unchanged.
