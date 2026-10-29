---
nav_exclude: true
---

# Accessibility text-span arithmetic

The accessibility text provider accepts integer start/end positions.  After
validating ordering and the widget’s text length, it now widens both endpoints
before subtracting them.  This prevents signed `int` overflow from defeating
the byte-capacity check for a very large requested range; the span is narrowed
only after it has been proven to fit the underlying Text/TextField API.

The source contract is checked by
`tests/automated/check-accessible-text-span-arithmetic.py`.
