---
nav_exclude: true
---
# Decoded image background fallback

When SVG or PNG decoding needs a background but no explicit appearance color
is available, ImageCache now uses `WhitePixelOfScreen(screen)`. Explicit
application background resources remain authoritative. This removes the old
pixel-zero fallback that could produce unexpected black regions in ordinary
Motif image and dialog-icon rendering.
