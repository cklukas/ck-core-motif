# XmString component-copy allocation

`XmStringGetNextComponent` now checks the temporary buffer used when copying
text components.  Overflow or allocation failure marks the iterator error and
returns `XmSTRING_COMPONENT_END` instead of copying through a null or wrapped
buffer.  The existing zero-copy path and successful component results are
unchanged.
