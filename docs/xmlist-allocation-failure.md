---
nav_exclude: true
---
# Internal list allocation failures

The internal `XmList` node allocator now checks its `XtCalloc` result before
initializing a free-node batch. If allocation fails, `_XmListAddAfter` and
`_XmListAddBefore` return `NULL` and leave the list links unchanged.
