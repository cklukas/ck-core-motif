# MWM confirmation-action bounds

`ConfirmAction()` now returns for a missing screen record, missing top-level
widget, or an action index outside the four built-in confirmation actions.
Valid confirmation dialogs retain their existing message, callback, centering,
and management behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-confirm-action-bounds.py
```
