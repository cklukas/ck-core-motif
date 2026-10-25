---
nav_exclude: true
---
Selection and prompt dialog argument allocation bounds
========================================================

`XmCreateSelectionDialog` and `XmCreatePromptDialog` append an internal
`XmNdialogType` argument to the caller's SelectionBox argument list.  The
temporary list now rejects counts that cannot represent the extra element or
its byte size in the legacy `Cardinal` allocator interface before any
multiplication or allocation.  Allocation failure and invalid non-empty source
lists return `NULL` before the temporary list is accessed.

As with the other Motif convenience constructors, an empty list is valid with
a null pointer.  The copy is skipped in that case, avoiding an undefined
zero-length `memcpy` while preserving the normal empty-argument behavior.
