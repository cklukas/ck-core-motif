---
nav_exclude: true
---
# ScrollBar slider pixmap

ScrollBar renders its slider into an off-screen pixmap before copying it into
the widget window.  If X11 cannot create that pixmap, the draw path now stops
before issuing fill, shadow, or mark requests against `None`; the existing
window-copy path continues to treat the missing optional pixmap as unavailable.
