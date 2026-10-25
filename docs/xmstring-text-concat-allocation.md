# XmString text concatenation allocation

`_XmStringGetTextConcat` now checks output-length arithmetic and the temporary
result of `XtRealloc` before copying a component.  A failed or unrepresentable
growth releases the partial output and context state and returns `NULL`.
