---
nav_exclude: true
---
# Text byte-conversion allocation

The two-byte Text source conversion path uses a small temporary byte buffer
while it expands each `BITS16` value.  If that allocation fails, the converter
now writes an empty result and returns instead of dereferencing a null pointer.
The normal conversion and terminator behavior is unchanged.

`tests/automated/check-text-byte-conversion-allocation.py` protects the guard
and its empty-result cleanup path.
