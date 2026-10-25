# CascadeButtonGadget pixmap metadata

CascadeButtonGadget glyph rendering now checks the result of
`XmeGetPixmapData` before using the returned depth.  If the pixmap metadata is
unavailable, the glyph copy is skipped safely instead of branching on an
uninitialized depth value.

Successful lookups retain the existing depth-matched `XCopyArea` or bitmap
`XCopyPlane` behavior.  The focused regression is
`tests/automated/check-cascade-gadget-pixmap-metadata.py`.
