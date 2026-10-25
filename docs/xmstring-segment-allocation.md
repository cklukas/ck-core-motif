---
nav_exclude: true
---

# XmString segment allocation

`_XmStringSegmentNew` now checks entry-array and segment-array allocation before
publishing mutations. Copy failures and allocation failures leave the existing
XmString structure usable, while successful segment insertion retains the
existing layout and ownership behavior.

The focused regression check is
`tests/automated/check-xmstring-segment-allocation.py`.
