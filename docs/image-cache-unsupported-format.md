---
nav_exclude: true
---
# Image-cache unsupported-format cleanup

Scaled pixmap creation now treats an unsupported `XImage` format as a normal
failure. It uses the existing cleanup path to restore image ownership, release
allocated pixels, and return `XmUNSPECIFIED_PIXMAP`; supported bitmap and
pixmap formats are unchanged.

The regression check is
`tests/automated/check-image-cache-unsupported-format.py`.
