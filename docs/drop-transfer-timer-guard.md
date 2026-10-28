Guard delayed drop-transfer timer state
=======================================

The delayed drop-transfer timer now returns safely when Xt invokes it without
its transfer object client data.  This keeps timer teardown consistent with
the other drop-transfer callbacks and prevents a null dereference before the
drag-context validity check.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-timer-input.py
```
