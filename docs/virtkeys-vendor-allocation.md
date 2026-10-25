---
nav_exclude: true
---
# Virtual-key vendor formatting allocation

Vendor-specific virtual-key binding loading now checks the server-vendor
value and its formatted vendor/version buffer before formatting. An allocation
failure simply skips vendor-file matching; generic fallback handling remains
available to the caller.

`tests/automated/check-virtkeys-vendor-allocation.py` protects this contract.
