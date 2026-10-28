---
nav_exclude: true
---

# UIL object enum-width boundaries

Object semantic values and definition flags use compact historical symbol
fields, while parser tables and diagnostic APIs use wider integer types. The
object semantic actions now make those allocation, enum, diagnostic, and flag
conversions explicit without changing their stored values.

Regression: `tests/automated/check-uil-object-enum-widths.py`
