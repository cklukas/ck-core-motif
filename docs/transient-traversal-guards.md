---
nav_exclude: true
---
# Transient traversal guards

Transient-tree traversal can be requested after a client has been detached.
`FindTransientTreeLeader` and `FindTransientOnTop` now return `NULL` for a
missing client, while `CountTransientChildren` returns zero.  Callers can
therefore skip restacking safely instead of dereferencing an invalid tree.
Valid traversal results are unchanged.
