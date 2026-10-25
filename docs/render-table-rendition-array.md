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
