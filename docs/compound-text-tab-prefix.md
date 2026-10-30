---
nav_exclude: true
---

# Compound-text tab prefixes

`GetTextSegment` can prepend tab characters without marking the segment as a
line separator. That path now leaves the text terminated only by its normal
NUL byte; it does not append a newline unless the compound-text separator flag
is set. This prevents a one-byte overwrite of the allocated output buffer and
preserves the distinction between indentation and a new line.

The focused regression check is
`tests/automated/check-compound-text-tab-prefix.py`.
