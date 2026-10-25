---
nav_exclude: true
---
# Tree extra-space geometry

Tree layout derives unused space by subtracting its bounding-box dimensions
from the widget dimensions.  The vertical and horizontal calculations now
convert both operands to signed integers before subtraction, so an oversized
or temporarily clipped tree reports the configured margin rather than a
wrapped near-maximum space value.
