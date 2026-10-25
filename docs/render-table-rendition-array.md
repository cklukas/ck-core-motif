# Render-table rendition arrays

`XmRenderTableGetRenditions` now follows its documented result layout: the
returned array has one slot for every requested tag, and an unmatched tag
leaves its corresponding slot as `NULL`. The previous implementation compacted
matches and resized the allocation, which made the public positional contract
unreliable for callers that requested more than one tag.

The allocation size is checked before multiplication, allocation failure is
reported as `NULL`, and a partially copied result is released before returning.
Successful matching and rendition ownership remain unchanged; callers still
release each non-`NULL` rendition with `XmRenditionFree` and the array with
`XtFree`.

`XmRenderTableCopy` applies the same body-size bound before allocating a copied
table for a large tag selection or full-table copy.

`XmRenderTableAddRenditions` rejects counts that cannot be represented by its
integer counters and checks the new-table and match-array sizes before
allocation. The append path now publishes its replacement table only after
the table handle and every copied rendition are available; failed allocation
or copying leaves the pre-append table available to the caller.

Shared-table merges similarly roll back the temporary body, handle and copied
renditions if any part of the detached copy cannot be allocated.

Removal uses a planning pass and publishes the compacted table only after its
replacement storage and copied rendition handles are ready. Allocation failure
therefore leaves the original table and its ownership unchanged; see
`docs/render-table-resize.md`.
