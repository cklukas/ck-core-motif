---
nav_exclude: true
---
# XmString segment-length bounds

`_XmStringGetSegment` exposes component lengths through the legacy `short`
output used by Motif's segment APIs. It now rejects components larger than
`SHRT_MAX` before assigning that output, preventing unsigned lengths from
wrapping negative and reaching allocation or copy code. Existing representable
segments retain their behavior.

The regression check is
`tests/automated/check-string-segment-copy-guards.py`.
