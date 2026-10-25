---
nav_exclude: true
---
# Mwm colormap-reference allocation failures

Mwm keeps per-window arrays of client references for colormap management. The
add and remove paths now check each `XtMalloc` before copying or publishing the
array. On allocation failure they report the condition and leave the existing
X context untouched, avoiding a null dereference during client property or
focus updates.

`tests/automated/check-colormap-reference-oom.py` guards all three allocation
failure paths.
