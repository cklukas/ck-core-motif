---
nav_exclude: true
---
# Compound-text CSI lookahead bounds

CSI decoding examines two parameter bytes after the current byte.  Both
compound-text decoder implementations now verify that at least three bytes
remain before that lookahead, so truncated CSI sequences fail cleanly instead
of reading beyond the input buffer.

`tests/automated/check-ctext-csi-bounds.py` protects both paths.
