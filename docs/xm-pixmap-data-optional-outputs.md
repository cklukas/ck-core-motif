---
nav_exclude: true
---
# Xm pixmap-data optional outputs

`_XmGetPixmapData` now validates the screen and pixmap, clears only supplied
outputs, and preserves the historical optional-output contract. Callers may
pass null output pointers, including metadata-only lookups used by Motif
widgets.

The source-level regression is
`tests/automated/check-xm-pixmap-data-optional-outputs.py`.
