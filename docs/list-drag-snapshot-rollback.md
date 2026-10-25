---
nav_exclude: true
---
# List drag snapshot rollback

List drag initiation now checks every selected-item `XmStringCopy`.  A failed
copy releases already-copied entries, the temporary snapshot, and the drag
context instead of starting a drag with NULL payload entries.  Completion also
clears the context after cleanup and safely ignores a duplicate completion
callback.

The focused regression is
`tests/automated/check-list-drag-allocation.py`.
