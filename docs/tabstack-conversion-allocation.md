---
nav_exclude: true
---
# TabStack conversion allocation

`TabConvertProc` now checks the temporary child-pointer allocation before
writing and publishing the conversion result.  If allocation fails, the
conversion refuses the request cleanly and leaves the drag/drop callback state
unchanged.

The focused source-level regression is
`tests/automated/check-tabstack-conversion-allocation.py`.
