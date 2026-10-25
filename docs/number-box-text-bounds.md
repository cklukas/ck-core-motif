---
nav_exclude: true
---

# Number-box text-length bounds

`XmNumberBox` now bounds editable text lengths before passing them to
`XDrawString`. Normal input rendering is unchanged; unusually long text no
longer wraps the signed Xlib length argument.

The focused regression check is
`tests/automated/check-number-box-text-bounds.py`.
