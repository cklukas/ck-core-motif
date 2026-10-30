# NumberBox value update clears invalid state

When an application supplies a new NumberBox value through resources, the
widget now clears any stale invalid-entry state before formatting and
redisplaying the value.  Programmatic correction therefore removes the error
appearance instead of leaving the control marked invalid.

The focused source-contract check is:

```sh
python3 tests/automated/check-numberbox-value-clears-invalid.py
```
