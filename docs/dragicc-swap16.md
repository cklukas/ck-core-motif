---
nav_exclude: true
---

# Drag ICC 16-bit byte swapping

The ICC `swap2bytes` macro now swaps the two object-representation bytes with
`memcpy`, preserving the established wire transformation for both signed
`INT16` coordinates and unsigned `CARD16` fields. It no longer assigns an
unsigned bit-field temporary back through a type-dependent conversion.

Regression: `tests/automated/check-dragicc-swap16.py`
