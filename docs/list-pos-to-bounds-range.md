---
nav_exclude: true
---
# List position-to-bounds range

`XmListPosToBounds` accepts the historical zero value to mean the last item,
but other values are one-based positions. The API now rejects negative and
out-of-range values before converting a valid nonzero position to zero-based
form, preventing signed overflow for extreme `int` inputs.

Regression coverage: `tests/automated/check-list-pos-to-bounds-range.py`.
