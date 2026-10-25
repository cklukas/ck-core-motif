# Label drag-icon pixmap metadata

Label and LabelG drag-icon creation now initializes pixmap width, height, and
depth outputs before querying metadata.  An invalid or unresolved pixmap thus
produces deterministic zero metadata rather than arbitrary drag-icon resource
values; valid pixmaps retain their existing dimensions and depth.

The focused regression is
`tests/automated/check-label-drag-pixmap-metadata.py`.
