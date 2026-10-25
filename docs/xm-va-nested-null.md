---
nav_exclude: true
---
Null nested varargs lists
=========================

The internal nested-varargs counting and conversion helpers now treat a null
`XtTypedArgList` as an empty list.  This keeps malformed or optional nested
list inputs from being dereferenced while preserving the normal conversion
result for valid lists.
