# Selection-loss cleanup

Selection-loss conversion callbacks now have explicit ownership handling. The
clipboard MOVE loss path releases the DELETE conversion result, and the normal
loss path initializes its result pointer before cleanup. A converter that
declines to return data therefore cannot leak or expose an uninitialized value
to `XtFree`.

The regression check is:

```sh
python3 tests/automated/check-transfer-lose-cleanup.py
```
