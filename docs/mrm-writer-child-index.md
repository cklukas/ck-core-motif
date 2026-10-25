---
nav_exclude: true
---
# Mrm writer child index input

`UrmCWRSetChild` now rejects a null index for indexed children before measuring
or appending it.  Empty indexes continue to return the existing
`MrmNULL_INDEX` error, while RID children and valid indexes retain their normal
UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-child-index.py`.
