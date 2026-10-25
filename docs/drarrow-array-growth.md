---
nav_exclude: true
---
# DrArrow geometry-array growth

`XmeDrawArrow` keeps three rectangle arrays for the arrow’s top, center, and
bottom sections.  The old code resized them independently and updated the
shared capacity even if one allocation failed.  Later geometry writes could
then use a null or shorter array.

Growth now allocates all three arrays, copies the previous contents, and
commits the replacement set only when every allocation succeeds.  A failed
growth leaves the previous geometry buffers and capacity intact.
