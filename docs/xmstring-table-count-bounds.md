# XmString table count bounds

The public XmString table conversion helpers allocate arrays from caller or
content-derived `Cardinal` counts.  The table-size products now use `size_t`
checks and reject values that cannot be represented by the Xt allocator's
`Cardinal` byte-count interface.  Content-derived splitting also uses a wide
counter and returns an empty result instead of overflowing an `int` counter.
Normal table conversions and their ownership rules are unchanged.
