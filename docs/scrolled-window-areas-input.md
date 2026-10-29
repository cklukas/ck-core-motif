# ScrolledWindow area-setter guard

`XmScrolledWindowSetAreas` now returns immediately for a NULL ScrolledWindow
widget before calling `XtSetValues`. The historical NULL child arguments still
mean “leave that area unchanged”; valid area assignment is unchanged.

The focused source contract is checked by
`tests/automated/check-scrolled-window-areas-input.py`.
