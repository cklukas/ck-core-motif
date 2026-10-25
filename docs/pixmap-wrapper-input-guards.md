---
nav_exclude: true
---
# Pixmap wrapper input guards

The public `XmGetScaledPixmap`, `XmGetPixmapByDepth`, and `XmGetPixmap`
wrappers now reject a null widget or screen, and a null image name, before
accessing Xt or X11 state. Invalid input returns `XmUNSPECIFIED_PIXMAP`;
valid calls retain their existing cache and locking behavior.

The focused regression is `tests/automated/check-xm-pixmap-wrapper-inputs.py`.
