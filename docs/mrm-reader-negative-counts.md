---
nav_exclude: true
---

# Mrm reader negative counts

Mrm text vectors, integer vectors, font lists, and legacy font-list fixups use
signed count fields. Their conversion paths now reject negative values before
iterating or exposing the item arrays, while retaining the existing
`RGMListSizeMax` upper bound for valid resource data.
