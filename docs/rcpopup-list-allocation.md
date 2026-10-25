# RowColumn popup-list allocation

`XmAddToPostFromList` keeps a private hash table and per-widget popup list for
automatic popup behavior.  The table and list allocations are now checked
before lookup, initialization, or insertion.  Allocation failure releases
both the process and application locks and leaves existing popup state
unchanged.

Successful popup registration and removal retain the existing API and list
ordering behavior.
