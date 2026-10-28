---
nav_exclude: true
---

# XPM token byte preservation

Both memory- and file-backed XPM token scanners now cast input through
`unsigned char` before storing it in the token buffer. High-bit symbols retain
their byte values consistently across platforms and strict conversion builds.

Regression: `tests/automated/check-xpm-token-byte-casts.py`
