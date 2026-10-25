---
nav_exclude: true
---

# Compound-text compatibility length tracking

The legacy `XmCvtCTToXmString(char *)` entry point receives a NUL-terminated
string rather than an explicit byte count.  Its parser now records that bound
before processing, rejects NULL input, and checks the optional four-byte
version header against it.

This makes the compatibility path safe for short or absent input while
preserving valid conversion behavior and the public ABI.  The static
regression check is `tests/automated/check-compound-text-compatibility-length.py`.
