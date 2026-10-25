# XmObjectAtPoint input validation

`XmObjectAtPoint` now returns `NULL` immediately for a null widget instead of
passing it to Xt class and application-context helpers.  Manager-widget
dispatch and object selection are unchanged for valid widgets.
