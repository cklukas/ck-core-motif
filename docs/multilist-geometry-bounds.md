---
nav_exclude: true
---
# MultiList geometry request bounds

MultiList now computes parent resize requests with signed width and height
intermediates, then clamps each result to one pixel before placing it in the
Xt geometry request.  Child shrink requests cannot wrap into huge parent
dimensions.

The focused regression is `tests/automated/check-multilist-geometry-bounds.py`.
