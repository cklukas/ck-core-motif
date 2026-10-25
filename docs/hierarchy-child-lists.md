# Hierarchy child-list snapshots

`XmHierarchyGetChildNodes` checks the child count before adding its terminating
`NULL` entry and allocating the returned widget list. The checked byte count is
passed explicitly as an Xt `Cardinal`; an unrepresentable count returns `NULL`
without wrapping the allocation size.


Child-list export now rejects sentinel-count wraparound before adding the
null terminator slot and returns cleanly if the list allocation is unavailable.
