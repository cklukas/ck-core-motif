---
nav_exclude: true
---
# Virtual-key vendor-string bounds

Virtual-key alias loading now checks the X server vendor-string length before
converting it to Xt's `Cardinal` allocation size. It reserves explicit space
for the release suffix and uses `snprintf`, rejecting a formatting failure
without leaking the temporary string.

The source contract is checked by:

```sh
python3 tests/automated/check-virtual-key-vendor-bounds.py
```
