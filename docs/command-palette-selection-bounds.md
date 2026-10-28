---
nav_exclude: true
---
# CommandPalette selection bounds

`XmCommandPalette` receives selected row numbers from `XmList`.  The invoke
callback now rejects an empty, zero, or out-of-range row before converting the
one-based list position to the zero-based visible-item index.  This keeps a
malformed or stale selection from underflowing the index or reading beyond the
filtered command map while preserving normal callback dispatch.

The source contract is checked by
`tests/automated/check-command-palette-selection-bounds.py`.
