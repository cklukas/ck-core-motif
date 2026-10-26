# StatusBar item-array allocation failure

`XmStatusBarSetItems` now validates item-count multiplication and checks the
`XtCalloc` result before creating child widgets.  An oversized or exhausted
allocation emits a warning and returns cleanly instead of dereferencing a
null item array.

The focused source-contract check is:

```sh
python3 tests/automated/check-statusbar-allocation-failure.py
```
