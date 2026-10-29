# StatusBar item-array allocation failure

`XmStatusBarSetItems` validates item-count multiplication before changing bar
state, so an oversized request is rejected without erasing existing status
items. It also checks the `XtCalloc` result before creating child widgets. An
exhausted allocation emits a warning and returns cleanly instead of
dereferencing a null item array.

The focused source-contract check is:

```sh
python3 tests/automated/check-statusbar-allocation-failure.py
```
