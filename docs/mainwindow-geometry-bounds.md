---
nav_exclude: true
---

# MainWindow geometry bounds

MainWindow now saturates geometry-manager width and height calculations before
placing them in X11 Dimension fields. Large child requests, borders, and
margins therefore cannot wrap into a smaller parent request.

The focused regression check is
tests/automated/check-mainwindow-geometry-bounds.py.
