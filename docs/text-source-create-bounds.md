# Text-source creation bounds

`_XmStringSourceCreate` now checks character-count and capacity growth before
performing source-buffer arithmetic.  Wide-character conversion storage and
the gapped text buffer are rejected when their sizes cannot be represented by
the allocator's `Cardinal` size, rather than wrapping and allocating a smaller
buffer than the input requires.

The focused source contract is checked by
`tests/automated/check-text-source-create-bounds.py`.
