# XmString entry-copy input validation

`_XmStringEntryCopy` now rejects malformed optimized or unoptimized entries
that advertise a positive text length while carrying no text pointer. This
prevents a `memcpy` through `NULL` when corrupted or partially constructed
internal data reaches a copy operation.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-entry-copy-input.py
```
