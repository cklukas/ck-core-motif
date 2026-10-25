---
nav_exclude: true
---
# Mrm writer callback routine input

`UrmCWRSetCallbackItem` now rejects a null routine name before measuring or
copying it.  Empty names continue to return the existing `MrmNULL_ROUTINE`
error, while valid callback names retain their normal UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-callback-routine.py`.
