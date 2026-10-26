---
nav_exclude: true
---

# Validate XPM file-image writes

`XpmWriteFileFromXpmImage` now rejects a null source image before opening the
destination.  A null filename remains the documented stdout output path, so
that compatibility behavior is unchanged.
