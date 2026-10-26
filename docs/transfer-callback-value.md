# Clipboard callback conversion values

`ClipboardCallback` now initializes and resets its conversion result before
each callback request. A declined conversion consequently leaves a known null
pointer for cleanup, preventing an uninitialized free while preserving the
existing deferred clipboard lifecycle.

The regression check is:

```sh
python3 tests/automated/check-transfer-callback-value.py
```
