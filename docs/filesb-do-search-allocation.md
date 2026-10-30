---
nav_exclude: true
---

# FileSelectionBox programmatic search allocation

`XmFileSelectionDoSearch` now checks copied and generated search strings before
measuring them. Allocation failure leaves the matching length at zero and
still follows the existing search cleanup path, avoiding a null
`XmStringLength` call for programmatic searches.

The focused source contract is checked by
`tests/automated/check-filesb-do-search-allocation.py`.
