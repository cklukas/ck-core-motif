---
nav_exclude: true
---

Always clean up after drag-context destruction
===============================================

`DragContextDestroyCB` now clears pending timers and selection requests even
after the delayed-start timer has fired.  The old `CR1146` early return used a
process-wide timer flag and could skip cleanup when that flag had already been
cleared, leaving requestor callbacks and transfer state registered during drag
teardown.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-drag-destroy.py
```
