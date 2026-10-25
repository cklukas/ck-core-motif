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

`XmRenderTableCopy` uses the internal rendition lookup while its table lock is
held, and rolls back copied handles if an allocation fails. Tagged copies
therefore avoid recursive locking and never return a partially initialized
table.
