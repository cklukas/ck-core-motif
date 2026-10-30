# Region operation allocation rollback

`miRegionOp` builds an operation result in replacement storage while retaining
the destination's previous rectangles until the operation succeeds. If the
scratch allocation fails, the destination now restores its original pointer,
rectangle count, and capacity instead of becoming empty and leaking the old
storage. Callers can therefore retry or destroy the region safely after an
out-of-memory failure.

The focused source-contract check is:

```text
python3 tests/automated/check-region-operation-allocation-rollback.py
```
