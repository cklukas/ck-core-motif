# Icon-directory cache allocation

Icon-directory cache growth now publishes a replacement directory array only
after `XtRealloc` succeeds, with checked count arithmetic. Directory-name and
cached-entry allocations are checked and return an uncached result cleanly
when memory is unavailable, leaving the process-wide cache consistent.

The regression check is
`tests/automated/check-icon-cache-growth-allocation.py`.
