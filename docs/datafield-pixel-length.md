---
nav_exclude: true
---
# DataField pixel measurement lengths

DataField pixel measurement now checks temporary Unicode conversion capacity
before calling `wcstombs` in Xft and core-font paths. Oversized or negative
lengths return a zero measurement instead of overflowing the temporary buffer.

Regression coverage: `tests/automated/check-datafield-pixel-length.py`.
