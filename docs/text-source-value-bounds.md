# Text source value bounds

Text source value extraction now rejects byte sizes that cannot be represented
by Xt's `Cardinal` allocator length, and it keeps assembled byte and wide
character counts in `size_t` storage. It also returns cleanly when called for
an unattached or incomplete source instead of dereferencing a missing widget.
Normal attached-source extraction and terminator behavior are unchanged.

The focused source contract is checked by
`tests/automated/check-text-source-value-bounds.py`.
