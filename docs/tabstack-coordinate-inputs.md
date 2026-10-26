# TabStack coordinate lookup input contract

`XmTabStackXYToWidget` accepts a widget from a tab stack, tab box, or tab
child and maps coordinates to the corresponding tab.  A null widget is not a
valid lookup target.  The function now returns `NULL` before touching the
application context, so optional callers can safely pass an absent widget
without crashing during teardown or failed construction.

The behavior for non-null widgets is unchanged.
