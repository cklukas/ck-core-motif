# Container conversion allocation guards

Container selection/drag conversion now checks target arrays, drag offsets,
selected-widget snapshots, and pixmap result arrays. Empty or unrepresentable
selection state refuses conversion before dereferencing a missing result.

The focused regression is
`tests/automated/check-container-conversion-allocation.py`.
