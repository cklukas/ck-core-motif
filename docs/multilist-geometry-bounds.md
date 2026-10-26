---
nav_exclude: true
---
# MultiList geometry request bounds

MultiList now computes parent resize requests with signed width and height
intermediates, clamps each result to one pixel before placing it in the Xt
geometry request, and saturates the frame's child-plus-border size during
construction. Child shrink requests and large border resources cannot wrap
into huge or unrelated parent dimensions.

The focused regression is `tests/automated/check-multilist-geometry-bounds.py`.
