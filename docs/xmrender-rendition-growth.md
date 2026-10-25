# XmRenderTable rendition-array growth

Property conversion accumulates created renditions in a dynamic array.  The
old code grew that array without checking the new capacity or `XtRealloc`'s
result, then continued to append.

The initial allocation and each growth are now checked.  A failed expansion
uses the existing parser cleanup path, retaining the already parsed prefix.
