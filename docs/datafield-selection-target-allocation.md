---
nav_exclude: true
---
# DataField selection-target allocation failures

`XmDataField` allocates response values for the `TARGETS` and `TIMESTAMP`
selection targets.  Those allocations now return `False` cleanly when memory
is unavailable, instead of dereferencing a null pointer while answering an
X11 selection request.  Existing successful responses and their ABI remain
unchanged.

The focused regression is
`tests/automated/check-datafield-selection-target-allocation.py`.
