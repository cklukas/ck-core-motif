# Stepper render-table replacement failures

`XmStepper` owns a copy of the render table used for its value text.  During a
resource update it first copies the requested table and only then releases the
old one.  If the copy cannot be allocated, the existing valid table remains in
place and the widget continues to be drawable.

The focused source contract is checked with:

```sh
python3 tests/automated/check-stepper-font-fallback.py
```
