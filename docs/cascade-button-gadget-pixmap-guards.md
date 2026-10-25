# CascadeButtonGadget pixmap guards

`XmCascadeButtonGadget` copies its cascade pixmap into the parent manager
window.  During resource replacement or manager transitions, the parent may
not be realized, the pixmap may be unspecified, or the label GC may be absent.

The copy path now validates those prerequisites before querying pixmap data or
issuing `XCopyArea`/`XCopyPlane`.  Valid cascade pixmaps retain their existing
position, size, and depth handling.
