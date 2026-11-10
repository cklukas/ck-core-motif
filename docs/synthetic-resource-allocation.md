---
nav_exclude: true
---
# Synthetic-resource allocation

Synthetic-resource class tables retain their initialized `XrmQuark` names in
the legacy `String` member.  The internal conversion now routes that tagged
value through `uintptr_t`, preserving the historical representation while
avoiding width-dependent direct pointer/integer casts on 64-bit builds.

`_XmBuildResources` now validates signed superclass/subclass counts, source
list pointers, and the complete byte-size calculation before allocating the
combined synthetic-resource table.  Failed allocation leaves the caller’s
existing list unchanged instead of copying through a null pointer.

The focused source-level regression is
`tests/automated/check-synthetic-resource-allocation.py`.
