---
nav_exclude: true
---
List selection-copy allocation bounds
=====================================

List item and selection-copy helpers now check both `size_t` and Xt's
`Cardinal` byte limits before allocating `XmString` pointer arrays.  This
prevents a large internal count from producing a byte size that the Xt
allocator interface cannot represent while preserving the existing
allocation-failure behavior.
