---
nav_exclude: true
---

# UIL include filename metadata bounds

Include symbol records retain a fixed 256-byte filename field for compatibility.
The parser now bounds and terminates the stored include name before copying it;
the separate temporary buffer still carries the complete name used to open the
file.

Regression: `tests/automated/check-uil-include-filename-bounds.py`
