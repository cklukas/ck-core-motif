---
nav_exclude: true
---
# MainWindow width query bounds

When a work window requests a resize, MainWindow queries its menu bar using
the available width after both margins.  That subtraction now uses signed
arithmetic and clamps collapsed windows to zero, preventing a wrapped query
width when the margins exceed the current window width.

The source-level regression is
`tests/automated/check-mainwindow-width-query-bounds.py`.
