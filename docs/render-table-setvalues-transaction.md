---
nav_exclude: true
---

# Remaining label/font SetValues ownership

BreadcrumbBar, Divider, and DropHighlight now validate replacement font or
label allocations before releasing the current resource. Allocation failure
keeps the existing value in place, avoiding a partially updated widget and
preserving the established Motif ownership contract.

The focused source contract is checked by
`tests/automated/check-render-table-setvalues-transaction.py`.
