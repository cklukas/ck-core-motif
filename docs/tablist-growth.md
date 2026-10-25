# TabList growth

`XmTabbedStackListInsert` now grows its attribute array through a temporary
pointer.  Slot-count and byte-size overflow are rejected, and an Xt allocation
failure returns `XmTAB_NOT_FOUND` while preserving the existing list and its
counts.

The focused regression is `tests/automated/check-tablist-growth.py`.
