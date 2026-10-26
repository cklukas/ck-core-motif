# Clipboard target-list cleanup

The clipboard source now releases a target-list conversion result when the
converter succeeds but returns an empty or non-`XA_ATOM` value. Successful
target lists transfer ownership to the loop explicitly, so neither path leaks
or double-frees the converter buffer.

The regression check is:

```sh
python3 tests/automated/check-transfer-target-list-cleanup.py
```
