---
nav_exclude: true
---
# Sash focus redraw lifecycle

Sash focus hooks can be requested by application traversal setup before the
widget is realized.  The highlight and unhighlight paths now defer their X
drawing until a window exists.  Once realized, their focus geometry and
appearance are unchanged.
