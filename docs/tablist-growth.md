# TabList growth

`XmTabbedStackListInsert` now grows its attribute array through a temporary
pointer.  Slot-count and byte-size overflow are rejected, and an Xt allocation
failure returns `XmTAB_NOT_FOUND` while preserving the existing list and its
counts.

`XmTabListInsertTabs` likewise rejects a `Cardinal` tab-count addition that
would wrap before copying the old list. `XmTabListFree` uses a `Cardinal`
iteration variable, so a valid full-width tab count cannot wrap a signed loop
counter during cleanup.

The focused regression is `tests/automated/check-tablist-growth.py`.
