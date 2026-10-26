# MWM command-line screen-name allocation

The `-screens` parser now keeps screen-name lengths in `size_t`, rejects names
that cannot fit Xt's `Cardinal` allocation size, and publishes `XtRealloc`
results only after success. The same bound is applied when copying the first
name to the remaining screens.

The source contract is covered by
`tests/automated/check-wm-resparse-screen-names.py`.
