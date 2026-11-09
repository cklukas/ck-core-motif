---
nav_exclude: true
---

# UIL database-header bounds

Compiled UIL database headers and global limits carry counts and byte sizes as
signed integers, but those fields feed allocation, lookup, and `fread` sizes.
Database loading rejects negative table fields before entering table dispatch
and negative global limits before publishing them. This prevents a corrupt
database from turning a signed underflow into a huge allocation or a poisoned
global loop bound. The loader returns after the diagnostic so this remains true
when an embedding application supplies a non-fatal diagnostic handler.

Regression: `tests/automated/check-uil-db-header-bounds.py`
