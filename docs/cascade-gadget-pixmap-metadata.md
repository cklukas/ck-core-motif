---
nav_exclude: true
---

# CascadeButtonGadget pixmap metadata guard

CascadeButtonGadget drawing now checks `XmeGetPixmapData` success before using
the returned depth.  Invalid or unavailable cascade pixmaps are skipped safely
while valid gadget rendering is unchanged.

The static regression check is
`tests/automated/check-cascade-gadget-pixmap-metadata.py`.
