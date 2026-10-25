---
nav_exclude: true
---
# Xm geometry wrapper null-safety

The private `_XmMoveWidget`, `_XmResizeWidget`, and `_XmConfigureWidget`
wrappers now return immediately for a null widget. Valid drag-and-drop geometry
updates still use the existing drop-site lifecycle and Xt operations. This
prevents teardown paths from invoking Xt with an invalid object.
