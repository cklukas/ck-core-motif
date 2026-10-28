---
nav_exclude: true
---

# XPM file-write status

File-backed XPM output now checks stream errors after the final write and
propagates `fclose()`/`stdout` flush failures through the public writer result.

Regression: `tests/automated/check-xpm-file-write-status.py`
