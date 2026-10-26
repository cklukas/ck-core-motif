---
nav_exclude: true
---

# Validate XPM pixmap-to-file inputs

`XpmWriteFileFromPixmap` now rejects a null display or a call with neither a
pixmap nor a shape mask before attempting X11 extraction.  Shape-mask-only
exports remain supported.
