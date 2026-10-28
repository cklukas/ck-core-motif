---
nav_exclude: true
---
# Scaled-pixmap depth input

The scaled-pixmap entry point now rejects depth zero before image loading or
X11 resource creation. X11 pixmaps require a positive depth; rejecting this
malformed input keeps callers on the normal `XmUNSPECIFIED_PIXMAP` failure
path instead of provoking an avoidable server-side error.

Regression coverage: `tests/automated/check-image-depth-input.py`.
