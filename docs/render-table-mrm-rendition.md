---
nav_exclude: true
---
# MRM rendition construction

`_XmCreateRendition` now rejects a null parent/table handle and a failed
rendition allocation before duplicate lookup or table mutation. Invalid MRM
construction therefore returns `NULL` without dereferencing partial state.

Table resizing and the copied table entry are checked as well; a failed
mutation releases the newly created rendition instead of publishing a null
slot.

The resized body count is also checked with `size_t` arithmetic before the
reallocation request.
