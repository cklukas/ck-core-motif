# Mrm pointer-list cleanup

`UrmPlistFree` and `UrmPlistFreeContents` now reject null list handles safely.
`UrmPlistFreeContents` also clears each freed slot and resets the active count,
so repeated cleanup cannot double-free stale entries and the list can be reused.
