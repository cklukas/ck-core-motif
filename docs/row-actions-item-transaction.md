---
nav_exclude: true
---
# RowActions item construction

`XmRowActionsSetItems` now validates all item and button-label allocations and
tracks the partial count while constructing buttons.  If an allocation or
widget creation fails, the partial list is destroyed and the widget remains
empty instead of dereferencing a null buffer or retaining an incomplete row.
Valid item arrays and button layout are unchanged.

The setter also ignores a replacement request after Xt begins destruction,
avoiding button construction and child management against teardown state.

`tests/automated/check-row-actions-item-transaction.py` protects this path.
