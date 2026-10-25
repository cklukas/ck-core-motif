---
nav_exclude: true
---
# Mrm header version allocation

`Idb__HDR_GetHeader` copies the compiled-in database version into writable
storage before parsing it because `sscanf` may update its input through
`ungetc`.  The copy now checks the `XtMalloc` result and reports `MrmFAILURE`
before dereferencing a null pointer.

The source-level regression is
`tests/automated/check-mrm-header-version-allocation.py`.
