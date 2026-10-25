# XmString context initialization allocation

`XmStringInitContext` now checks its context-record allocation and returns
`FALSE` when the record cannot be created.  It no longer initializes through a
null pointer under allocation pressure.
