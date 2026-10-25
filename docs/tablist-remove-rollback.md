# Transactional tab-list removal

`XmTabListRemoveTabs` now performs its mark-and-sweep operation on a copied
list. If copying fails, it returns the original list unchanged. Successful
removal frees the old list only after the new list has been prepared, and the
all-tabs-removed case releases both lists before returning `NULL`.
