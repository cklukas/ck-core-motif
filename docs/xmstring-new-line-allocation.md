---
nav_exclude: true
---

# XmString substring line allocation

Substring construction now creates line entries and grows the line array
transactionally. Allocation failure is reported through the string context and
the partial result is released instead of being dereferenced or published.

The focused regression check is
`tests/automated/check-xmstring-new-line-allocation.py`.
