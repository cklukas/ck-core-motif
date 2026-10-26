# Drop-site query input contract

`XmDropSiteRegistered` reports whether a widget has a registered drop site.
The widget is required to identify the display and drop-site manager.  The
query now returns `False` immediately for a null widget, before application
context or display lookup, so optional drag-and-drop cleanup can query safely.

The result for valid widgets is unchanged.
