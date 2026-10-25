# Separator drawing guards

`XmeDrawSeparator` is shared by menus, lists, scrollbars, and other widgets.
Its simple-line paths previously subtracted an unsigned margin from the
requested span before checking whether any line could fit. Shadowed paths also
passed missing top or bottom GCs into Xlib.

The helper now computes the line span in signed arithmetic, skips collapsed
spans, and checks the required separator or shadow GC before drawing. Valid
separator geometry and the public API are unchanged. The regression
`tests/automated/check-draw-separator-guards.py` covers these guards.
