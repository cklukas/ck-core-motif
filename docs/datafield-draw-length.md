---
nav_exclude: true
---
# DataField Unicode drawing lengths

DataField Unicode drawing now checks length multiplication before allocating
temporary conversion buffers and passes the checked `size_t` capacity to
`wcstombs`. Oversized or negative lengths return without overflowing legacy
`int` and `Cardinal` conversions.

Regression coverage: `tests/automated/check-datafield-draw-length.py`.
