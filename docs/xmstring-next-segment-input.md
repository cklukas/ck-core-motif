# XmString next-segment input validation

`XmStringGetNextSegment` now rejects a null context or output pointer before
entering the segment conversion path. Valid callers and allocated text/tag
ownership remain unchanged; malformed calls return `False` without touching
the output objects.
