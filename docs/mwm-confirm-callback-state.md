# MWM confirmation callback state

`OkCB()` now returns safely when callback data is missing or its action number
is outside the four built-in confirmation handlers.  The dialog is still
withdrawn and the mapped flag cleared in those cases; valid callbacks retain
their existing action dispatch.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-confirm-callback-state.py
```
