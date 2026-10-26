---
nav_exclude: true
---
# Text selection property sizes

STRING, COMPOUND_TEXT, and UTF-8 selection exports now reject X text-property
payloads larger than Xt’s `Cardinal` allocation width before copying them.
This prevents large external/property data from truncating the allocation size.

Regression coverage: `tests/automated/check-text-selection-property-size.py`.
