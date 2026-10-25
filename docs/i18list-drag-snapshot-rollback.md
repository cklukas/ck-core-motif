# I18List drag snapshot rollback

I18List drag initiation now checks its context, row-array, and `XmString`
allocations. Partial row snapshots are released on failure, and column-header
drags copy their title so completion owns every string it frees. Cleanup is
NULL-safe and clears the widget's drag context after release.

The focused regression is
`tests/automated/check-i18list-drag-snapshot-rollback.py`.
