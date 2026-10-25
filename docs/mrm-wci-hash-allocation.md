---
nav_exclude: true
---
# Mrm widget-class hash allocation

Widget-class hash insertion now rejects null table/name inputs, checks the
length before narrowing to the legacy hash function’s `int` parameter, and
handles entry-size overflow or allocation failure before linking a new node.
Valid registration and lookup ordering remain unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-hash-allocation.py`.
