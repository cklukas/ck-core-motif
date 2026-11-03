# MWM callback ABI safety

`AdjustTearOffControl` returns a `Boolean` when MWM calls it directly to find
out whether menu geometry changed. Xt cascading callbacks, however, use the
void-return `XtCallbackProc` ABI. MWM now registers a void-return adapter that
discards the callback-only result while preserving the direct Boolean call.

This avoids invoking a function through an incompatible pointer type on ABIs
where function return conventions are not interchangeable.

The focused source-contract regression is:

```sh
python3 tests/automated/check-mwm-callback-types.py
```
