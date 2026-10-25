# XmStringPutRendition source-copy failure

`XmStringPutRendition` now checks the source-string copy before concatenating
the opening rendition marker.  A failed copy releases that marker and returns
null instead of producing a string containing only rendition boundaries.
