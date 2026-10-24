# Mrm pointer-list content cleanup state

`UrmPlistFreeContents` now rejects a missing vector or inconsistent active and
slot counts before walking the list. This protects cleanup of partially
initialized lists while preserving repeatable cleanup for valid lists.
