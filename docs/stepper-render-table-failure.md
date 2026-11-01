# Stepper render-table replacement failures

`XmStepper` owns a copy of the render table used for its value text.  During a
resource update it first copies the requested table and only then releases the
old one.  If the copy cannot be allocated, the existing valid table remains in
place and the widget continues to be drawable.

Native font inheritance and child override behavior are checked with the
following command. This suite does not inject render-table allocation failure:

```sh
python3 tests/automated/run-stepper-native.py --prefix PREFIX --build-dir BUILD --out OUTPUT
```
