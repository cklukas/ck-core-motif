# DataField modify-verify buffer

When a wide `XmDataField` value is passed to a narrow modify-verify callback,
the implementation creates a temporary multibyte buffer. That allocation is
now checked before conversion or callback dispatch. An allocation failure
rejects the proposed edit safely and leaves the existing value available for
the normal restoration path.

The regression check is:

```sh
python3 tests/automated/check-datafield-verify-buffer.py
```
