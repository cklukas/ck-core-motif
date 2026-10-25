---
nav_exclude: true
---
# Scrolled-list argument merge failure

`XmCreateScrolledList` now stops cleanly if `XtMergeArgLists` cannot allocate
the combined argument list. The temporary child name is released before the
failure is returned, and normal widget construction is unchanged.

`tests/automated/check-scrolled-list-arg-merge.py` protects this contract.
