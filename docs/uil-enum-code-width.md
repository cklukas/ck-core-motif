---
nav_exclude: true
---

# UIL enumeration-code width

UIL keyword enumeration codes are held in a compact 16-bit value-entry field.
The semantic assignment now makes that representation boundary explicit while
retaining the existing range assertion and value lookup.

Regression: `tests/automated/check-uil-enum-code-width.py`
