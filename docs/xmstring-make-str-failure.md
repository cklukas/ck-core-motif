# XmString context slicing failures

`MakeStr` now checks whether `_XmStringSegmentNew` actually inserted each
copied segment.  If allocation fails and the new string remains incomplete,
the context is marked erroneous, the partial string is freed, and null is
returned.
