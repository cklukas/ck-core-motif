# CascadeButton pixmap binding metadata

CascadeButton and CascadeButtonGadget now distinguish a valid uncached pixmap
metadata fetch from an unresolved pixmap. Their glyph drawing paths validate
the initialized depth instead of treating `XmeGetPixmapData`'s cache-status
Boolean as an error signal, so the first draw of a valid glyph is retained.

The focused regression is `tests/automated/check-cascade-pixmap-binding.py`.
