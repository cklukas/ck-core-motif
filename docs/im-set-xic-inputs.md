# XIC setter input contract

`XmImSetXIC` associates or queries an input context for a widget.  A null
widget has no registration to update; the function now returns `NULL` before
application-context lookup.  Valid association and query behavior is
unchanged.
