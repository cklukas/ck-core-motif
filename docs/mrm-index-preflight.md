# Mrm index insertion preflight

`Idb__INX_EnterNodeIndex` now checks the neighboring child links before
shifting entries, writing the new index, or changing free-space metadata.  A
malformed B-tree therefore fails without leaving a partially inserted node.
Successful insertions retain the existing ordering and parent-repair behavior.

The source-level regression is `tests/automated/check-mrm-index-preflight.py`.
