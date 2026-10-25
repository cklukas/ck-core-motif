---
nav_exclude: true
---
# Mrm index insertion bounds

Leaf and node insertion now validate record counts, heap offsets, free-space
metadata, and insertion positions before moving index vectors or subtracting
string storage.  Unexpected search results are propagated safely, preserving
normal B-tree insertion while rejecting malformed in-memory records.

The source-level regression is
`tests/automated/check-mrm-index-entry-bounds.py`.
