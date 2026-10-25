---
nav_exclude: true
---
# Mrm index collapse copy bounds

The leaf and node index collapse helpers now compute a bounded source length
and rounded storage size before copying a string into their temporary heap.
Oversized or corrupt aggregate strings are rejected before the copy can write
past the allocation, while valid serialized indexes retain their existing
layout.

The source-level regression is
`tests/automated/check-mrm-index-collapse-copy-bounds.py`.
