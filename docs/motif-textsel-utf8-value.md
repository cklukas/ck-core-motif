# Text UTF-8 value allocation failure

UTF-8 selection conversion now checks its returned-value allocation, releases
the Xlib property buffer on failure, and rejects the conversion with zero
length.

The regression check is
`tests/automated/check-textsel-utf8-value-allocation.py`.
