---
nav_exclude: true
---
# TextOut position-to-coordinate source validation

`_XmTextPosToXY` now rejects a text-source block that does not advance the
position or reports bytes without a source pointer. This avoids an infinite
loop and prevents invalid blocks from reaching width measurement.

Regression coverage: `tests/automated/check-textout-postoxy-source-progress.py`.
