# Hierarchy child-list snapshots

`XmHierarchyGetChildNodes` now checks the child count before adding its
terminating `NULL` entry and allocating the returned widget list. An
unrepresentable count returns `NULL` without wrapping the allocation size.
