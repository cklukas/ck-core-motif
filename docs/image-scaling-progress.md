---
nav_exclude: true
---
# Image-scaling strip progress

Scaled images are copied in destination-height strips.  Rounding can collapse
several source rows to one destination coordinate for extreme dimensions.  A
final strip must still advance; if the row table reaches its end without
progress, the copy now exits through the normal cleanup path instead of
repeating the same strip indefinitely.

The focused regression is `tests/automated/check-image-scaling-progress.py`.
