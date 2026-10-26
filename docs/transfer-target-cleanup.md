# Clipboard target cleanup

The clipboard source resets the conversion output before requesting each
target and after freeing it. A failed conversion therefore cannot reuse the
previous target's freed pointer during cleanup, preventing a double-free while
retaining the normal per-target ownership contract.

The regression check is:

```sh
python3 tests/automated/check-transfer-target-cleanup.py
```
