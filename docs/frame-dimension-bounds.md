---
nav_exclude: true
---

# Frame dimension bounds

Frame sizing now saturates width, height, title extent, and title spacing at
the X11 Dimension limit. Title and shadow origins also remain representable
Positions. Large child borders, margins, shadows, or preferred sizes therefore
cannot wrap a Frame into unexpectedly small geometry.

The focused regression check is
tests/automated/check-frame-dimension-bounds.py.
