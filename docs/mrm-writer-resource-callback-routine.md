---
nav_exclude: true
---
# Mrm writer resource callback routine input

`UrmCWRSetCallbackItemRes` now rejects a null routine name before measuring or
copying it.  Empty names continue to return the existing `MrmNULL_ROUTINE`
error, while valid resource callback names retain normal UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-resource-callback-routine.py`.
