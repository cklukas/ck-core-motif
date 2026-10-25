---
nav_exclude: true
---
# XmRegion operation input guards

Private region operations now handle optional geometry state consistently:
null sources are treated as empty, null destinations are left untouched, and
null rectangle inputs or shrink targets are ignored. Valid region operations
retain their existing intersection, union, subtraction and shrink behavior.

The focused regression is
`tests/automated/check-region-operation-input-guards.py`.
