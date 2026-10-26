# Toolbar action-array allocation failure

`XmToolbarSetActions` now checks the action count before allocation and handles
an `XtCalloc` failure before creating child widgets or dereferencing the new
array.  The old action model is already released by the API's existing
replacement semantics; an oversized or exhausted request now returns cleanly
with a warning instead of crashing.

The focused source-contract check is:

```sh
python3 tests/automated/check-toolbar-allocation-failure.py
```
