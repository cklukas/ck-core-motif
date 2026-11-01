---
nav_exclude: true
---
# Hash-table accounting limits

The internal Motif hash table exposes its number of entries as a `Cardinal`.
Insertion now fails once that counter reaches its representable maximum, so a
very large table cannot wrap its reported count to zero. Existing insertions,
lookups, removal order, and table sizing are unchanged.

When compiled with `DEBUG`, an empty table's diagnostic summary now reports a
zero average chain length instead of dividing by zero.

The focused regression is `tests/automated/check-xm-hash-lifecycle.py`.
