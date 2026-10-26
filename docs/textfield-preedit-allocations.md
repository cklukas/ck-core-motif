---
nav_exclude: true
---

# TextField preedit allocation failures

TextField's XIM preedit path creates temporary multibyte, wide-character, and
tab buffers.  The path now checks every allocation before writing into it and
releases the already-created buffer if the second empty-preedit allocation
fails.  Valid preedit rendering is unchanged; low-memory callbacks return
cleanly instead of dereferencing NULL.

`tests/automated/check-textfield-preedit-allocations.py` protects these guards.
