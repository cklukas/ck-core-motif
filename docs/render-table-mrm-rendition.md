---
nav_exclude: true
---
# MRM rendition construction

`_XmCreateRendition` now rejects a null parent/table handle and a failed
rendition allocation before duplicate lookup or table mutation. Invalid MRM
construction therefore returns `NULL` without dereferencing partial state.
