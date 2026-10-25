---
nav_exclude: true
---
# Legacy resource-string allocation

The compatibility file-selection restore path and row/column menu-accelerator
resource getter now check their temporary string allocations before copying.
Allocation failure leaves the caller with no replacement string instead of
writing through `NULL`; successful legacy resource behavior is unchanged.

`tests/automated/check-obsolete-resource-allocation.py` protects this
contract.
