# Appearance transaction snapshot bounds

Appearance-profile changes snapshot all live widget states before preparing a
transaction.  The snapshot count is tracked in native `size_t`, rather than
wrapping through the public `Cardinal` type, and the snapshot plus its
per-widget change plans reject a count that cannot fit both the `XtMalloc`
`Cardinal` argument and native allocation arithmetic. The transaction returns
failure before taking any state references; valid transactions are unchanged.
After that validation, the allocation is explicitly narrowed to Xt's accepted
byte-count type.

The modern arrow inset helper also clamps its position updates to the X11
`Position` range, so large allocated cells cannot wrap the arrow origin.
The helper also treats a missing widget or geometry pointer as a no-op before
consulting display/profile state.

The focused source-contract check is:

```sh
python3 tests/automated/check-appearance-snapshot-bounds.py
```
