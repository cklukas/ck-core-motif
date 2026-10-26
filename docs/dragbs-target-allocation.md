# Drag-and-drop target-table allocation guards

`_XmTargetsToIndex` sorts a target list and may grow the display's target
table. It now bounds both array sizes, checks the initial target allocation,
and uses a temporary pointer for table growth so a failed `XtRealloc` leaves the
old table intact. Every failure path frees the new target list and releases the
server/process lock before returning.

The focused source-contract check is
`tests/automated/check-dragbs-target-allocation.py`.
