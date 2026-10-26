# Clipboard MOVE cleanup

The clipboard source path initializes its conversion value and clears it before
the final MOVE delete conversion. If that conversion declines to provide a
value, cleanup now still passes a known null pointer to `XtFree` instead of an
uninitialized pointer.

The regression check is:

```sh
python3 tests/automated/check-transfer-move-cleanup.py
```
