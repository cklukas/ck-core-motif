---
nav_exclude: true
---

# Pixel converter input validation

Rendition-pixel converters now validate displays, argument counts, source and
destination values before conversion or warning dispatch. The shared integer
parser also safely rejects missing strings or result pointers. Valid pixel and
special-value conversions remain unchanged.

The direct `XmRPixel` converter now copies a complete `Pixel` object after
checking the supplied source width. It no longer reads the resource through an
`int`, which could truncate a 64-bit pixel value or reinterpret a high-bit
color as a negative signed integer.

The focused regression check is
`tests/automated/check-pixel-converter-inputs.py`.
