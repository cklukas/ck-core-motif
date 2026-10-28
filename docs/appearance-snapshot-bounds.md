# Appearance transaction snapshot bounds

Appearance-profile changes snapshot all live widget states before preparing a
transaction.  The snapshot and its per-widget change plans now reject a count
that cannot fit both the `XtMalloc` `Cardinal` argument and the native `size_t`
allocation arithmetic.  The transaction returns failure before taking any
state references; valid transactions are unchanged.

The modern arrow inset helper also clamps its position updates to the X11
`Position` range, so large allocated cells cannot wrap the arrow origin.
The helper also treats a missing widget or geometry pointer as a no-op before
consulting display/profile state.

The focused source-contract check is:

```sh
python3 tests/automated/check-appearance-snapshot-bounds.py
```
