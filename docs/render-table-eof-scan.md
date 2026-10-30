---
nav_exclude: true
---
# Render-table property EOF handling

The initial item-list scan in `XmRenderTableCvtFromProp` now stops on either a
newline or end-of-input token.  A malformed property without a trailing
newline can no longer leave the scan index unchanged and loop forever.

The regression check is `tests/automated/check-render-table-eof-scan.py`.
