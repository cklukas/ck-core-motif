# Text preedit wide conversion

Wide-character preedit input now checks that its multibyte conversion buffer
fits the allocator's `Cardinal` size and that allocation succeeds before
calling `wcstombs`.  Failure restores the insertion-point display and returns
without dereferencing unavailable storage.

The focused source contract is checked by
`tests/automated/check-text-preedit-wide-conversion.py`.
