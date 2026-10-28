---
nav_exclude: true
---

# Mrm callback-writer terminators

`UrmCWRSetArgCallback` and `UrmCWRSetCreationCallback` append a null callback
item for the in-memory Xt callback list.  Their descriptor allocation now
includes that terminator (`nitems + 1`) and validates the complete size before
writing it.  Valid UID writer output remains unchanged apart from eliminating
the one-item overwrite at the boundary.
