---
nav_exclude: true
---

# Select-color converter input validation

The select-color converter now validates display, argument, source, and
destination inputs before handling special constants or invoking Xt's pixel
converter. Converter-data writes are conditional, while valid special-color
and ordinary pixel conversions remain unchanged.

The focused regression check is
`tests/automated/check-select-color-converter-inputs.py`.
