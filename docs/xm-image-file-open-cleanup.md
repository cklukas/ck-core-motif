---
nav_exclude: true
---
# Xm image-file open cleanup

When JPEG/PNG support is enabled and the resolved image path cannot be
opened, `GetImage` now frees the path returned by `XmGetIconFileName` before
returning failure. Successful and other format paths retain their existing
ownership behavior.

The source-level regression is
`tests/automated/check-xm-image-file-open-cleanup.py`.
