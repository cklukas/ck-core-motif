---
nav_exclude: true
---
# VendorShell warning buffers

`MotifWarningHandler` builds a combined header/message string and an indented
copy for `XtWarning`. Both buffers now validate their byte sizes against the Xt
allocation limit and check allocation results before formatting or copying.
Oversized or allocation-limited warnings return without dereferencing null
storage.
