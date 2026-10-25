# Scaled pixmap dual-table insertion

Scaled pixmap entries are published in both the pixmap and pixmap-data hash
tables. Publication is now transactional: if either bucket insertion fails,
the first insertion is removed, the X11 pixmap is released, and the pending
entry is cleaned up instead of leaving inconsistent cache state.

The focused regression is `tests/automated/check-pixmap-dual-insertion.py`.
