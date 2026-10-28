Guard source-notification teardown reentrancy
==============================================

The source-notification callback now returns safely when its transfer object is
already marked for destruction.  Any Xt-owned result buffer is still released,
but the callback no longer clears state or schedules a second widget destroy
during deferred teardown.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-source-notified.py
```
