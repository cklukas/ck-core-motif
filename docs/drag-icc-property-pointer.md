---
nav_exclude: true
---

# ICC drag property pointer validation

ICC drag initiator and receiver property readers now require a non-null
returned data pointer in addition to the existing type, format, length, and
byte-order checks.  Malformed property replies are rejected before their
serialized structures are dereferenced.

Regression: `tests/automated/check-drag-icc-property-pointer.py`
