# Xft UTF-8 compound-string metrics allocation

The ISO-10646 core-font path used for UTF-8 compound strings converts the
input to UCS-2 before measuring it.  If that conversion cannot allocate its
temporary buffer, metrics now return without calling `XTextExtents16` with a
null pointer.  Successful conversions are released with `XtFree`, matching
the allocator used by `_XmUtf8ToUcs2`.
