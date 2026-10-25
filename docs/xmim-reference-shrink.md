---
nav_exclude: true
---
# XmIm reference-list shrinking

The input-method display record keeps parallel widget-reference and callback
arrays.  The old shrink path resized them independently and reduced the
recorded capacity before checking either allocation, allowing a failed second
resize to leave inconsistent metadata.

Shrinking now allocates both replacement arrays, copies the active prefix,
and commits the pair only when both allocations succeed.  Otherwise the
larger existing arrays remain valid.
