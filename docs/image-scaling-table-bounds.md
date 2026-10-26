---
nav_exclude: true
---
# Image-scaling table bounds

The scaled-image copy loop walks the source-to-destination row table while
rendering each strip.  It now explicitly stops at `image_height`, rather than
depending only on the final rounded coordinate to terminate the loop.  This
keeps malformed or precision-edge scaling inputs inside the allocated table
even when a server reports unusual image dimensions.

The focused regression is
`tests/automated/check-image-scaling-table-bounds.py`.
