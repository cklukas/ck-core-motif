# Xm pixmap hotspot output initialization

`_XmGetPixmapData` returns cached pixmap geometry and optional image hotspot
metadata. When the cache had no matching `ImageData` entry, it returned
`True` without assigning `hot_x` or `hot_y`, allowing callers such as
`XmGetPixmapData` to observe indeterminate coordinates.

The cache lookup now initializes both hotspot outputs to zero before checking
optional image metadata. Existing metadata still replaces those defaults,
while cache hits without metadata have deterministic coordinates.
