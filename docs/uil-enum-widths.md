---
nav_exclude: true
---

# UIL enum-width boundaries

UIL parser enum values are wider in local temporaries than the historical
symbol-table fields and diagnostic APIs. The conversions are now explicit at
those boundaries, preserving the existing ABI-sized fields and diagnostic
arguments while making the intended representation visible to strict builds.

Regression: `tests/automated/check-uil-enum-widths.py`
