---
nav_exclude: true
---
# List drag target allocation failure

List drag conversion builds target arrays for TARGETS and export-target
requests.  Both allocations are now checked before atoms are appended.  If
allocation fails, conversion reports `XmCONVERT_REFUSE` without writing
through null storage.

The regression check is
`tests/automated/check-list-drag-target-allocation.py`.
