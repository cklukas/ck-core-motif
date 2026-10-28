---
nav_exclude: true
---

# Drag ICC wire widths

Callback timestamps, windows, and ICC handles are serialized into Motif's
32-bit client-message fields. The conversion is now explicit at every callback
serialization site, preserving the established ICC wire format while making
the protocol boundary visible to strict compilers and reviewers.

Regression: `tests/automated/check-dragicc-wire-widths.py`
