---
nav_exclude: true
---
# Transient sibling guards

Transient sibling ordering can run while a parent or sibling is being
withdrawn.  `PutTransientOnTop` now stops when the requested client is not
found in its parent’s chain, and `PutTransientBelowSiblings` handles an empty
child list.  Both helpers also reject a null client, preserving valid
restacking behavior while preventing malformed-chain dereferences.
