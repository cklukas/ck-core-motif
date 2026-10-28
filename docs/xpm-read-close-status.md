---
nav_exclude: true
---

# XPM file-to-buffer close status

`XpmReadFileToBuffer` now treats a failed `fclose()` as an unsuccessful read,
while retaining the existing short-read and VMS empty-record checks.
The validated file size is also converted once to `size_t` before allocation
and reading, avoiding signed-width diagnostics on large files.

Regression: `tests/automated/check-xpm-read-close-status.py`
