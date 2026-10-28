---
nav_exclude: true
---

# Text SelectScan pointer ownership

`SelectScan` now uses its existing character pointer in the byte-oriented word
selection branch instead of redeclaring a shadowing local. This keeps the
control flow unambiguous and allows strict `-Wshadow` builds to validate the
function without suppression.

Regression: `tests/automated/check-text-selectscan-shadow.py`
