# RowActions item-array bounds

`XmRowActionsSetItems` now checks the requested item count before allocating
its stored-item array.  The count must fit both native `size_t` multiplication
and Xt's `Cardinal` allocation width; an oversized request emits the existing
warning-style failure and leaves the widget out of its initialization phase.

The focused source-contract check is:

```sh
python3 tests/automated/check-row-actions-allocation-bounds.py
```
