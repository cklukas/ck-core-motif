---
nav_exclude: true
---
# RowActions item-array allocation failure

`XmRowActionsSetItems` validates the item-count multiplication before entering
its replacement phase. An oversized request therefore leaves the current
action strip intact instead of erasing it first. Allocation failures after
replacement begins retain the existing behavior: the strip remains valid and
can be populated by a later call.

The focused source-contract check is
`tests/automated/check-row-actions-input-preservation.py`.
