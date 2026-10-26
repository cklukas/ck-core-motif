---
nav_exclude: true
---

# Bound DnD buffer reads

`_XmReadDragBuffer` now performs its cursor and remaining-size arithmetic in
`size_t` and uses subtraction-based bounds checks.  A wrapped `numCurr + size`
calculation can no longer bypass the clamp and copy past the received property
buffer.
