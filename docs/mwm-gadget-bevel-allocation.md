# MWM gadget bevel allocation failure

`DepressGadget()` uses two small reusable rectangle lists to paint a title-bar
gadget.  The old short-circuit allocation expression could leave one list null
and still fall through to drawing.  The helper now allocates each missing list
independently and returns `FALSE` when either allocation is unavailable; the
next call can retry the missing list without changing successful drawing.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-gadget-bevel-allocation.py
```
