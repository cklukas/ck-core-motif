---
nav_exclude: true
---

# Label pixmap drag-icon input validation

`GetPixmapDragIcon` now returns no icon for a null widget before querying its
screen or pixmap. Normal Label pixmap drag-icon creation is unchanged.

The focused regression check is
`tests/automated/check-label-drag-icon-input.py`.
