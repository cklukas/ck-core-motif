# XIM query input contract

`XmImGetXIM` returns the input-method connection associated with a widget.
When no widget is available, there is no display or registration to query;
the function now returns `NULL` before application-context lookup.  Valid XIM
queries are unchanged.
