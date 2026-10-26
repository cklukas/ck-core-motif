# Text-source highlight-copy bounds

When a second widget attaches to a shared text source, its highlight list is
grown to match the first widget.  The allocation size is now checked against
both `size_t` and Xt's `Cardinal` limit before the multiplication and cast.
Oversized highlight metadata is rejected without issuing a truncated
allocation request.

The focused source contract is checked by
`tests/automated/check-text-source-highlight-copy-bounds.py`.
