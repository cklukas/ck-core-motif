---
nav_exclude: true
---

# XPM serialization structure

The data, buffer, and file serializers now reject malformed `XpmImage`
structures before calculating output sizes or walking color and pixel arrays.
They require positive dimensions and chars-per-pixel, a nonzero color count,
and present color-table and pixel-data arrays.  Valid images retain their
existing output formats.

Regression: `tests/automated/check-xpm-serialization-structure.py`
