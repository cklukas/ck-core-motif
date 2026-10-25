---
nav_exclude: true
---

# Hash-index native-width preservation

Hash-table bucket selection now converts `XmHashValue` to `unsigned long`
before taking the modulo. This preserves the upper bits of pointer-derived
hashes on 64-bit systems, reducing avoidable collisions without changing the
table API or lookup semantics.

The focused regression check is
`tests/automated/check-hash-index-width.py`.
