---
nav_exclude: true
---
# ColorSelector list allocation

The color selector lazily creates its scrolled color list while loading the
RGB database. If that list cannot be created, loading now releases the merged
argument list and returns before managing the child, updating list resources,
or installing callbacks. Normal list creation and color-file loading remain
unchanged.

The contract is protected by
`tests/automated/check-color-selector-list-allocation.py`.
