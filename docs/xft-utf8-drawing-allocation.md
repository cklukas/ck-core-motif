# Xft UTF-8 compound-string drawing allocation

When a two-byte core font draws a UTF-8 compound-string segment, Motif first
converts the segment to UCS-2.  Both image and normal drawing paths now skip
the draw if that temporary conversion cannot allocate, rather than passing a
null pointer to Xlib.  Successful conversions are released with `XtFree`,
matching `_XmUtf8ToUcs2`.
