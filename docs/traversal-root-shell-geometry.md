---
nav_exclude: true
---

# Traversal geometry for root shells

Traversal's rectangle calculation handles realized top-level shells with no
widget parent. Their coordinates are translated from the shell itself, rather
than passing a null parent to `XtTranslateCoords`. Child widgets and gadgets
retain their existing parent-relative geometry and border offsets.

This fixes a crash during focus visibility calculations when other windows
obscure a control whose parent is the application shell, including Wsm's
workspace panel. The native `traversal-root-shell.c` regression checks root
shell, child widget and gadget rectangles against Xt coordinate translation.
It runs as part of `run-mwm-workspace-integration.py`.
