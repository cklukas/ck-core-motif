# Text XA_STRING value allocation failure

Text selection conversion now checks the returned-value allocation for
`XA_STRING`. On failure it releases the Xlib property buffer, clears the
length, and rejects the conversion without exposing partial data.

The regression check is
`tests/automated/check-textsel-string-value-allocation.py`.
