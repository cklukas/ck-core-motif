# Text compound-text value allocation failure

Compound-text selection conversion now checks its returned-value allocation,
releases the Xlib property buffer on failure, and rejects the conversion with
zero length.

The regression check is
`tests/automated/check-textsel-compound-value-allocation.py`.
