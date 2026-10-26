# Paned separator allocation guards

When the Paned widget enables separators during a resource update, it builds a
temporary child-pointer array for the managed panes. The allocation now checks
the signed pane count and both native and Xt `Cardinal` multiplication limits,
and rejects the update if the temporary array cannot be allocated. This avoids
writing through a wrapped or NULL array while retaining the existing separator
creation behavior for valid pane counts.

The focused source-contract check is
`tests/automated/check-paned-separator-allocation.py`.
