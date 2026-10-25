# Hierarchy child-list snapshots

`XmHierarchyGetChildNodes` checks both sentinel-count and
`(child_count + 1) * sizeof(Widget)` overflow before adding its terminating
`NULL` entry and allocating the returned widget list. It also iterates with a
`Cardinal` index, matching the full child-count range. An unrepresentable
count returns `NULL` without wrapping the allocation size, and allocation
failure returns cleanly.
