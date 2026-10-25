---
nav_exclude: true
---

# Unit-converter input validation

Horizontal and vertical position/dimension converters now validate their Xt
argument, widget, source string, and destination before resolving screen units.
Valid unit conversion and warning behavior remain unchanged.

The focused regression check is
`tests/automated/check-unit-converter-inputs.py`.
