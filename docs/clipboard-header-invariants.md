---
nav_exclude: true
---

# Clipboard header invariants

Clipboard header validation now requires a non-zero item capacity, ensures
the current item count does not exceed that capacity, and requires the item
list offset to begin after the fixed header record.  Corrupt headers are
therefore rejected before later clipboard operations form list pointers or
grow the item count.

Regression: `tests/automated/check-clipboard-header-invariants.py`
