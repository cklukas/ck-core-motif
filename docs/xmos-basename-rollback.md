---
nav_exclude: true
---
# Xmos basename conversion rollback

The compatibility path in `_XmOSGetDirEntries` now allocates the basename
copy before releasing the full-path temporary.  If that allocation fails, the
temporary is released exactly once and the scan stops without publishing a
partial entry or dereferencing a lost pointer.

The focused regression is
`tests/automated/check-xmos-basename-rollback.py`.
