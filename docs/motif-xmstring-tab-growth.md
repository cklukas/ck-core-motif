# XmString proposed tab-list growth failure

When proposed tab-list measurement needs another tab, the new tab allocation
is checked before linking it into the list.  Failure frees the active string
context and tab list, releases the process lock, and returns `NULL`.

The regression check is
`tests/automated/check-xmstring-tab-growth-allocation.py`.
