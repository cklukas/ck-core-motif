# Empty region operation results

Region operations overwrite their destination. When both input regions are
empty, `miRegionOp` now clears the destination's rectangle count and extents
instead of returning with stale geometry from an earlier operation. Arithmetic
overflow and allocation failures still preserve the previous destination so a
caller can handle the failure without data loss.

The focused source-contract check is:

```text
python3 tests/automated/check-region-empty-operation.py
```
