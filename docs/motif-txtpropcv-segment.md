---
nav_exclude: true
---
# Text segment temporary buffers

`GetTextSegment` creates temporary buffers for locale and ISO-8859-1 segment
copies and for separator or tab insertion.  Each allocation is now checked;
failures return `_INVALID_SEGMENT` after releasing any partial result instead
of writing through null storage.  A negative length from the legacy signed
segment-length output is rejected before allocation, preventing an
unrepresentable long component from becoming a huge unsigned size.

The regression check is
`tests/automated/check-txtpropcv-segment-allocation.py`.
