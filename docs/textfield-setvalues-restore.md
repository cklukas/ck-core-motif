# TextField SetValues restoration

`XmTextField` runs modify-verify callbacks before accepting a resource update.
When a callback vetoes the update, SetValues restores the previous string. The
restore path now keeps the temporary `XtRealloc` result and copies only after
the allocation succeeds. This prevents a failed allocation from passing NULL
to `memcpy`; if the old allocation cannot be restored, the proposed value is
kept instead of corrupting memory.

The regression check is:

```sh
python3 tests/automated/check-textfield-setvalues-restore.py
```
