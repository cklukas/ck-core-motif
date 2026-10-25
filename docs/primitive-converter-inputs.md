---
nav_exclude: true
---

# Primitive string-converter input validation

Character and keysym converters now reject missing source/output values, and
the character converter also rejects an empty source buffer before reading its
first byte. Valid conversions and destination-size handling remain unchanged.

The focused regression check is
`tests/automated/check-primitive-converter-inputs.py`.
