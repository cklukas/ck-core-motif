---
nav_exclude: true
---

# TextField preedit reallocation safety

The overstrike preedit path grows byte and wide-character buffers for the
remaining text and recovered text. It now reallocates through temporary
pointers and checks failure before replacing the live buffer. A failed growth
therefore releases the old buffer and other temporary storage cleanly instead
of leaving a lost pointer followed by a NULL write.

`tests/automated/check-textfield-preedit-realloc.py` protects the transactional
reallocation pattern.
