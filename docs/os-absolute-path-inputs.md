---
nav_exclude: true
---
# Motif path-resolution inputs

`_XmOSAbsolutePathName` now rejects null path, output-pointer, and scratch
buffer arguments before accessing them. Valid absolute and relative paths keep
their existing behavior; malformed icon/resource lookup inputs return failure
instead of dereferencing invalid pointers.
