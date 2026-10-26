---
nav_exclude: true
---

# Validate XPM buffer-export arguments

Pixmap-, image-, and raw-image-to-buffer constructors now reject null output
records and empty source inputs before X11 extraction or buffer allocation.
Shape-only conversion remains supported, and optional `XpmInfo` metadata is
unchanged.
