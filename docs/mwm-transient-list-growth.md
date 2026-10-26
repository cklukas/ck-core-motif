# MWM transient-window list growth

The transient-window collector grows its `Window` list while walking siblings
and children. Growth now uses a temporary pointer and updates the size only
after `XtRealloc` succeeds. A recursive growth failure therefore leaves the
caller with the original list and consistent size/count state. The top-level
terminator growth frees its owned list on failure instead of losing the pointer.

The behavior is covered by
`tests/automated/check-transient-list-growth.py`.
