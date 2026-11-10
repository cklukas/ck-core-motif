# XmString entry-copy input validation

`_XmStringEntryCopy`, `EntryCvtToOpt`, and `EntryCvtToUnopt` reject malformed
unoptimized entries that advertise a positive text length while carrying no
text pointer. This prevents a `memcpy` through `NULL` when corrupted or
partially constructed internal data reaches a copy or representation-conversion
operation. `_XmStringOptToNonOpt` retains its null-string and destination
allocation guards; optimized string text is inline storage, so it has no
nullable text-pointer state to validate. The conversion reports failure when
its destination segment cannot be allocated.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-entry-copy-input.py
```
