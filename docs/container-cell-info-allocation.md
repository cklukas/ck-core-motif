---
nav_exclude: true
---
# Container cell-layout allocation

`XmContainer` uses temporary per-child metadata while calculating an ideal
size for `XmCELLS` layouts.  The allocation is now checked for size overflow
and failure before the child list is traversed.  If the metadata cannot be
created, sizing returns safely instead of dereferencing a null array; normal
layouts and their compatibility behavior are unchanged.

`tests/automated/check-container-cell-info-allocation.py` protects this
contract.
