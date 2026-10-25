---
nav_exclude: true
---

# Numeric resource-converter input validation

Boolean-dimension, cardinal, text-position, and top-item-position converters
now reject missing source/destination values (and missing widget context for
unit conversion) before parsing or dereferencing them. Valid numeric and
boolean conversions retain their existing behavior.

The focused regression check is
`tests/automated/check-numeric-converter-inputs.py`.
