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

The bucket-size table and all bucket indexes now use `Cardinal` consistently;
they no longer pass through signed `int` temporaries during resize, lookup,
or iteration.
Hash-table growth also checks the allocation width relevant to Xt's
`Cardinal` allocator, avoiding a redundant host-`SIZE_MAX` comparison.
