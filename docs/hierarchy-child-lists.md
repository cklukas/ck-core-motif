# Hierarchy child-list snapshots

`XmHierarchyGetChildNodes` now checks the child count before adding its
terminating `NULL` entry and allocating the returned widget list. An
unrepresentable count returns `NULL` without wrapping the allocation size.


Child-list export now rejects sentinel-count wraparound before adding the
null terminator slot and returns cleanly if the list allocation is unavailable.
