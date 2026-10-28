---
nav_exclude: true
---

# XPM whitespace byte safety

XPM token scanning now classifies whitespace through an EOF-safe helper that
casts input bytes to `unsigned char`. Extension-name whitespace uses the same
byte-safe cast, avoiding undefined behavior for high-bit input bytes.

Regression: `tests/automated/check-xpm-whitespace-byte-safety.py`
