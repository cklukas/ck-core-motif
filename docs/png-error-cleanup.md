# PNG decoder error cleanup

libpng reports malformed input and allocation failures through `longjmp`.
The loader now tracks decoded image and row-pointer allocations across that
boundary, releases both on error, and keeps `png_read_end` ahead of row cleanup
so late parser errors are covered too.

`tests/automated/check-png-error-cleanup.py` protects this ownership contract.
