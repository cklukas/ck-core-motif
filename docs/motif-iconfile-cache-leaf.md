# Icon-file cache leaf allocation

Derived icon cache entries now release their directory, key, and record if the
leaf-name copy fails.  The optional cache is skipped while the resolved icon
path remains available to the caller.

The regression check is `tests/automated/check-iconfile-leaf-allocation.py`.
