---
nav_exclude: true
---
# DataField replacement allocation failures

Text replacement now keeps the existing character buffer intact when
`XtRealloc` fails. The temporary result is checked before replacing the live
widget pointer, and the operation reports failure without losing existing
text.

Regression coverage: `tests/automated/check-datafield-realloc-failure.py`.
