# Standard client-window conversion allocation

`XmeStandardConvert` allocates a `Window` result for the
`CLIENT_WINDOW` target. If that allocation fails, conversion now reports an
empty `XA_WINDOW` result and releases the application lock before returning,
instead of dereferencing a null pointer.

The focused source contract is checked by
`tests/automated/check-standard-convert-client-window-allocation.py`.
