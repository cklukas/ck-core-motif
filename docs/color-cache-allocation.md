# Dynamic default-color cache allocation

The Mwm dynamic color cache now checks growth reallocation before publishing a
new cache pointer.  If growth fails, the cache remains valid and the dynamic
default conversion falls back to the requested background pixel instead of
dereferencing a null result.

`tests/automated/check-color-cache-allocation.py` protects this path.
