---
nav_exclude: true
---
# Xm scaled-pixmap table initialization failure

`_XmGetScaledPixmap` now stops with `XmUNSPECIFIED_PIXMAP` when lazy pixmap
cache initialization cannot publish both required tables. This prevents a null
hash lookup while retaining the existing successful cache path.

The source-level regression is
`tests/automated/check-xm-scaled-pixmap-table-null.py`.
