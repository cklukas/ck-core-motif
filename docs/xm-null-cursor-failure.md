---
nav_exclude: true
---
# Xm null-cursor failure handling

`XmeGetNullCursor` now checks both the temporary pixmap and cursor creation
results. It releases the application lock and returns `None` without caching
invalid state when the X server cannot create either resource.

The source-level regression is `tests/automated/check-xm-null-cursor-failure.py`.
