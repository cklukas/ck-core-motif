---
nav_exclude: true
---
# XmIm reference-list shrinking

The input-method display record keeps parallel widget-reference and callback
arrays. The old shrink path resized them independently and reduced the
recorded capacity before checking either allocation, allowing a failed second
resize to leave inconsistent metadata.

Shrinking now allocates both replacement arrays, copies the active prefix,
and commits the pair only when both allocations succeed. Otherwise the
larger existing arrays remain valid.

Removal searches with the same unsigned `Cardinal` type as the stored count,
so a valid large list is not truncated by an `int` comparison. The shrink
threshold is expressed as `n <= (max - 1) / 3` instead of multiplying `n`,
which avoids unsigned wraparound while preserving the historical `3*n < max`
threshold. The public XIM/Motif interfaces remain unchanged.
