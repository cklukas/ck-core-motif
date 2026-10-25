# FontSelector family-table growth

`XmFontSelector` grows its internal family table while processing the server's
XLFD inventory.  Growth now checks signed-capacity arithmetic and both host
`size_t` and Xt `Cardinal` byte limits before changing the capacity or calling
`XtRealloc`.  If the next table cannot be represented, processing stops with
the already-built table intact.

The focused regression is `tests/automated/check-fonts-family-growth.py`.
