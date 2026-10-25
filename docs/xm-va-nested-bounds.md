---
nav_exclude: true
---
Nested typed varargs bounds
===========================

Nested `XtVaNestedList` entries can expand into more records than their outer
list contributes.  The typed-varargs converter now carries the remaining
capacity through recursive expansion and rejects a nested list that would
exceed the allocated array.  It also rejects null nested lists, frees the
temporary array, and reports an empty result on failure.

Successful nested-list conversion remains unchanged.
