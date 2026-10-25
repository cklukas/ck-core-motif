---
nav_exclude: true
---
# Render-table rendition arrays

`XmRenderTableGetRenditions` now follows its documented result layout: the
returned array has one slot for every requested tag, and an unmatched tag
leaves its corresponding slot as `NULL`. The previous implementation compacted
matches and resized the allocation, which made the public positional contract
unreliable for callers that requested more than one tag.

The allocation size is checked against both host `size_t` and Xt `Cardinal`
byte limits before multiplication. Allocation failure is reported as `NULL`,
and a partially copied result is released before returning.
Successful matching and rendition ownership remain unchanged; callers still
release each non-`NULL` rendition with `XmRenditionFree` and the array with
`XtFree`.

Display-less tables are protected by the process lock. Allocation-failure
cleanup releases copied renditions through the internal lock-held helpers, so
it does not recursively acquire the process lock.

`XmRenderTableAddRenditions` checks the new-table body, handle, and initial
rendition copies before publishing the table. If any allocation fails, already
copied renditions are released and the call returns `NULL`.

Merge modes also retain the existing rendition when a replacement or clone
cannot be allocated; no merge mode publishes a null rendition slot.

When copying a shared table for a merge, body/handle/rendition failures now
roll back the temporary table and leave the original shared table unchanged.

Appending unmatched renditions follows the same transactional rule: failed
body, handle, or entry copies are released without publishing a partial table.

Count narrowing and allocation-size arithmetic are checked before creating the
temporary match array or a new table body.

`XmRenderTableCopy` applies the same body-size bound before allocating a copied
table for a large tag selection or full-table copy.

`XmRenderTableCopy` uses the internal rendition lookup while its table lock is
held, and rolls back copied handles if an allocation fails. Tagged copies
therefore avoid recursive locking and never return a partially initialized
table.

`XmRenderTableAddRenditions` also checks its temporary merge-match array
before clearing it, returning the original table if that allocation fails.
