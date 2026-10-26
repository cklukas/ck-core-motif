---
nav_exclude: true
---

# Validate XPM pixmap-to-data inputs

`XpmCreateDataFromPixmap` now rejects a null display, output-array pointer, or
empty pixmap/shape-mask pair before attempting X11 extraction.  Shape-mask-only
conversion remains supported.
