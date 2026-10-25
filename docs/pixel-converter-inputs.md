---
nav_exclude: true
---

# Pixel converter input validation

Rendition-pixel converters now validate displays, argument counts, source and
destination values before conversion or warning dispatch. The shared integer
parser also safely rejects missing strings or result pointers. Valid pixel and
special-value conversions remain unchanged.

The focused regression check is
`tests/automated/check-pixel-converter-inputs.py`.
