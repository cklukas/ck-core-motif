---
nav_exclude: true
---
Convenience-constructor allocation failures
============================================

The argument arrays used by the Command and message-dialog convenience
constructors are temporary allocations owned by the constructor.  If Xt
cannot provide that storage, the constructors now return `NULL` before the
array is copied or indexed.  This keeps allocation failure on the normal
widget-creation error path instead of dereferencing a null temporary array.

The change does not alter the public argument-list ABI or the successful
creation path.
