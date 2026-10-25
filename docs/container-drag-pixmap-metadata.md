# Container drag-icon pixmap metadata

Container drag-icon creation now initializes the width, height, and depth
outputs passed to `XmeGetPixmapData`.  An invalid or unresolved pixmap thus
produces zero-sized metadata instead of propagating uninitialized values into
the drag-icon resources.  Valid pixmap metadata follows the existing path.

The focused regression is
`tests/automated/check-container-drag-pixmap-metadata.py`.
