# Text highlight snapshot growth

Before changing a highlight range, `XmText` keeps a snapshot so redisplay can
compare the old and new regions.  Snapshot growth now checks the temporary
`XtRealloc` result before changing the stored capacity or marking the snapshot
valid.  Allocation failure unlocks the application context and leaves the
previous snapshot state intact instead of allowing a null dereference.

The focused source contract is checked by
`tests/automated/check-text-highlight-snapshot-growth.py`.
