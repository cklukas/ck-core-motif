---
nav_exclude: true
---
# Drag target-table serialization guards

`WriteTargetsTable` now rejects tables that cannot be represented by the
serialized 16-bit list count, non-empty lists with null atom arrays, and size
arithmetic that would overflow `size_t`.  Large-property allocation is also
checked before the buffer is filled, preventing wrapped sizes and null
dereferences during cache publication.
