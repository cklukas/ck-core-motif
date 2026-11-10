# RowActions item-array bounds

`XmRowActionsSetItems` now checks the requested item count before allocating
its stored-item array. The public count must fit Xt's `Cardinal` allocation
width; an oversized request emits the existing warning-style failure and
leaves the widget out of its initialization phase. The check uses this
effective `UINT_MAX` bound directly, avoiding a redundant `SIZE_MAX`
comparison that strict compilers diagnose as unreachable for `Cardinal`.

The focused source-contract check is:

```sh
python3 tests/automated/check-row-actions-allocation-bounds.py
```
