Release skipped transfer-selection values
==========================================

`SelectionCallbackWrapper` now tracks whether a selection result was delivered
to an application callback.  Results arriving after cancellation, for ignored
transfer blocks, without a transfer block, or without a callback procedure are
released by the wrapper instead of leaking the Xt-owned selection buffer.
Delivered results retain the existing callback ownership contract and are not
freed by the wrapper.

The focused regression is:

```text
python3 tests/automated/check-transfer-skipped-value-cleanup.py
```
