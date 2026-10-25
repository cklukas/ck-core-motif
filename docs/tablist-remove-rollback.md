---
nav_exclude: true
---
# Transactional tab-list removal

`XmTabListRemoveTabs` now performs its mark-and-sweep operation on a copied
list. If copying fails, it returns the original list unchanged. Successful
removal frees the old list only after the new list has been prepared, and the
all-tabs-removed case releases both lists before returning `NULL`. The removal
position loop uses a `Cardinal` index matching its public position-count
argument, so a large valid position list cannot wrap a signed counter.
