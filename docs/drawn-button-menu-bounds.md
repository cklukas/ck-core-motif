# DrawnButton menu bounds

DrawnButton menu activation and unhighlight callbacks can run while a widget
has a very small allocated size.  Their border-clearing rectangles now require
positive interior dimensions and convert the unsigned core dimensions before
subtraction.  The menu arm shadow path also checks both shadow GCs before
calling the X11 shadow helper.

Normal menu feedback keeps the existing colors, timing, and shadow geometry.
Only undersized or partially initialized widgets skip the affected drawing
operation.  The focused regression is
`tests/automated/check-drawn-button-menu-bounds.py`.
