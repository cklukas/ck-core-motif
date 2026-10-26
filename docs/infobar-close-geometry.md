---
nav_exclude: true
---
# InfoBar close-button geometry

The InfoBar close button now uses an 18-pixel square only when the widget is
at least 24 by 18 pixels, keeping the frame and X glyph inside the widget.
Pointer activation uses the same geometry threshold and horizontal origin.
Font-list replacement is transactional when `SetValues` cannot copy the new
render table.

The focused source check is
`tests/automated/check-infobar-close-geometry.py`.
