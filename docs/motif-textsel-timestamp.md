# Text selection timestamp allocation failure

Text selection conversion returns the selection timestamp through a temporary
`Time` object.  The allocation is now checked before the timestamp is written.
If it fails, conversion returns `False` with a null value so the request is
refused cleanly instead of dereferencing null storage.

The regression check is
`tests/automated/check-textsel-timestamp-allocation.py`.
