---
nav_exclude: true
---
# Image-scaling target depth

Scaled images are rendered into a temporary `XImage` before being copied to
the destination drawable.  The temporary image now uses the destination
drawable's server-reported depth as well as its screen visual.  This avoids a
visual/depth mismatch on displays whose drawable depth differs from the
source image.  If geometry cannot be queried, the source image depth remains
the compatibility fallback.

The focused regression is
`tests/automated/check-image-scaling-depth.py`.
