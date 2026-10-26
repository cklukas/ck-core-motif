---
nav_exclude: true
---
# TextOut pixmap-name bounds

Text output derives internal I-beam and add-mode cursor pixmap names from
integer geometry values. The fixed name buffers now use `snprintf`, preventing
an unexpectedly large geometry value from overrunning the name storage.

The source contract is covered by
`tests/automated/check-textout-pixmap-name-bounds.py`.
