---
nav_exclude: true
---

# MainWindow geometry bounds

MainWindow now saturates geometry-manager and preferred-size width and height
calculations before placing them in X11 Dimension fields. Large child requests,
borders, margins, highlights, scrollbars, or separators therefore cannot wrap
into a smaller parent request.

The focused regression check is
tests/automated/check-mainwindow-geometry-bounds.py.
