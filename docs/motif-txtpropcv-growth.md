---
nav_exclude: true
---
# Text-property conversion result growth

`GetUseableText` accumulates converted segments in `final_string`.  Growth is
now performed through a temporary pointer and checked before the result is
replaced.  On failure, the current segment, partial result, and string context
are released and the conversion reports `XNoMemory`.

The regression check is
`tests/automated/check-txtpropcv-growth-allocation.py`.
