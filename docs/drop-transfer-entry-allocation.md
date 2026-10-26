# Bound drop-transfer entry work arrays

Before dispatching a drop-transfer list, Motif allocates arrays for target
atoms and callback data.  The process now checks both allocation products,
handles zero-entry lists without allocating, and releases the first array if
the second allocation fails.  Truncated sizes and null-array writes are
therefore avoided.

The focused source regression is
`tests/automated/check-drop-transfer-entry-allocation.py`.
