---
nav_exclude: true
---
# Mrm writer CString-vector rebind failures

`UrmCWRSetArgCStringVec` appends each element to the resource record. An
append can relocate the record, so the argument descriptor must be rebound
before writing the element offset. The setter now checks that per-element
rebind and returns the failure instead of continuing with stale pointers.

The source-level regression is
`tests/automated/check-mrm-writer-cstring-vector-rebind.py`.
