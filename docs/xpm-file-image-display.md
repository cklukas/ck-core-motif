---
nav_exclude: true
---
# XPM file-to-image display validation

`XpmReadFileToImage` requires an X display for image creation and color
queries. A null display is rejected before file parsing or Xlib calls.

The source contract is covered by
`tests/automated/check-xpm-file-image-display.py`.
