# Handle drop-site manager allocation failure

Drop-site updates and visible-child queries now check their Xt allocations.
Queued update creation returns cleanly when memory is unavailable, while
visible-child results reject size overflow and return an empty result instead
of dereferencing a null array.

The focused source regression is
`tests/automated/check-dropsite-allocation-failure.py`.
