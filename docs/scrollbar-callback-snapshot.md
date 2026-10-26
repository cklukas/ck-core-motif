# ScrollBar callback snapshots

ScrollBar callback dispatch snapshots the callback list so callbacks may
mutate or destroy the original list during delivery.  The snapshot now checks
the callback count and both allocation-width limits before copying, and cleanly
skips delivery if the temporary allocation fails instead of dereferencing a
null pointer.

The focused source-contract check is:

```sh
python3 tests/automated/check-scrollbar-callback-snapshot.py
```
