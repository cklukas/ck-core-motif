---
nav_exclude: true
---
# XmString table count bounds

The public XmString table conversion helpers allocate arrays from caller or
content-derived `Cardinal` counts.  The table-size products now use `size_t`
checks and reject values that cannot be represented by the Xt allocator's
`Cardinal` byte-count interface.  Content-derived splitting also uses a wide
counter and returns an empty result instead of overflowing an `int` counter.
The public parse, unparse, and concatenation loops use `Cardinal` indices to
match their full input count range, avoiding signed-index wraparound for large
but representable tables. Normal table conversions and their ownership rules
are unchanged.
