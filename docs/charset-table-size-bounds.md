---
nav_exclude: true
---
# Charset-table allocation bounds

The string-to-charset-table resource converter stores both the pointer table
and copied charset names in one Xt allocation.  It now checks that the table
itself fits in the allocator's `Cardinal` width before subtracting its size
from `UINT_MAX`; this prevents unsigned wraparound from admitting an
unrepresentable allocation on wide or malformed input.

`tests/automated/check-charset-table-size-bounds.py` is the focused source
regression for the guard.
