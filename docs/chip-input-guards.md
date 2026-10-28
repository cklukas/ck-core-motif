---
nav_exclude: true
---

# Chip action input guards

`XmChipInvoke` and `XmChipRemove` now reject null widget handles before calling
the subclass predicate.  Valid sensitive Chip actions are unchanged, while
invalid public calls no longer risk dereferencing a null Xt object.

`tests/automated/check-chip-input-guards.py` protects the guard order.
