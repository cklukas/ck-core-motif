---
nav_exclude: true
---
# Virtual-key fallback allocation

Virtual-key fallback loading now skips failed filename construction before
calling `fopen`, checks both built-in binding copies, and returns without
calling `strlen` when no fallback buffer could be allocated. Successful
vendor and generic fallback selection is unchanged.

`tests/automated/check-virtkeys-fallback-allocation.py` protects this
contract.
