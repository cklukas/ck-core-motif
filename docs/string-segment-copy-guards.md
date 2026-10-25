---
nav_exclude: true
---
# Compound-string segment copy guards

`_XmStringGetSegment` now checks allocations made when copying a segment’s tag
and text. A failed copy returns `FALSE`, clears the affected outputs, and
releases a partially copied tag so callers do not receive invalid pointers or
leak temporary data.
