---
nav_exclude: true
---
# RowColumn popup-list allocation

`XmAddToPostFromList` keeps a private hash table and per-widget popup list for
automatic popup behavior. The table and list allocations are now checked
before lookup, initialization, or insertion. Post-from-list preparation keeps
the old list until a bounded replacement is available, releases the old list
after copying, and leaves existing popup state unchanged on failure.

Successful popup registration and removal retain the existing API and list
ordering behavior.
