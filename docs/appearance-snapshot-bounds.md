# Appearance transaction snapshot bounds

Appearance-profile changes snapshot all live widget states before preparing a
transaction.  The snapshot and its per-widget change plans now reject a count
that cannot fit both the `XtMalloc` `Cardinal` argument and the native `size_t`
allocation arithmetic.  The transaction returns failure before taking any
state references; valid transactions are unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-appearance-snapshot-bounds.py
```
