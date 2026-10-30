# XmString entry-copy input validation

`_XmStringEntryCopy`, `EntryCvtToOpt`, `EntryCvtToUnopt`, and
`_XmStringOptToNonOpt` now reject
malformed optimized or unoptimized entries that advertise a positive text
length while carrying no text pointer. This prevents a `memcpy` through
`NULL` when corrupted or partially constructed internal data reaches a copy or
representation-conversion operation. The optimized-to-general conversion
also reports failure when its destination segment cannot be allocated.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-entry-copy-input.py
```
