# CascadeButton pixmap draw guards

`XmCascadeButton` copies its cascade pixmap into the widget window.  The old
path could reach the X copy helpers with an unrealized window, zero height,
an unspecified pixmap, or a missing label GC during resource replacement.

The copy path now validates those prerequisites before querying pixmap data or
issuing `XCopyArea`/`XCopyPlane`.  Valid cascade pixmaps retain their existing
position, size, and depth selection.
