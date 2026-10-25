# Icon-directory cache allocation

Icon-directory cache growth now publishes a replacement directory array only
after `XtRealloc` succeeds, with checked count arithmetic.  Valid directory
entries accumulate the name heap in `size_t` and check the complete
offset-array plus name-heap size before allocation.  Directory-name and
cached-entry allocations are checked and return an uncached result cleanly
when memory is unavailable, leaving the process-wide cache consistent.

`tests/automated/check-icon-cache-growth-allocation.py` protects these paths.
