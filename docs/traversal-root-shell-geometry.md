---
nav_exclude: true
---

# Traversal geometry for root shells

Traversal's rectangle calculation handles realized top-level shells with no
widget parent. Xt's `XtTranslateCoords` requires a parented widget, so the
shell window is translated to its root with `XTranslateCoordinates`; if the
window is not realized or cannot be translated, its requested root-relative
geometry is retained. Child widgets and gadgets retain their existing
parent-relative geometry and border offsets.

This fixes a crash during focus visibility calculations when other windows
obscure a control whose parent is the application shell, including Wsm's
workspace panel. The native `traversal-root-shell.c` regression checks root
shell, child widget and gadget rectangles against the appropriate Xt/Xlib
coordinate translation, then exercises focus traversal through the root shell.
It runs as part of `run-mwm-workspace-integration.py`.
