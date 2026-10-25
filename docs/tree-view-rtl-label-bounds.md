---
nav_exclude: true
---

# Tree-view RTL label bounds

`XmTreeView` now bounds depth offsets, RTL label-width, and X drawing arithmetic
before converting geometry to signed integers. Normal labels retain their
existing placement; deeply indented or unusually long labels no longer wrap the
calculated position.

The focused regression check is
`tests/automated/check-tree-view-rtl-label-bounds.py`.
