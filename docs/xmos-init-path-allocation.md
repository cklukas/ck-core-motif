---
nav_exclude: true
---
# XmOS initialization path allocation

`_XmOSInitPath` now checks all generated path allocations, including the
absolute-path copy and the environment/default path forms, before formatting
or copying into them. Allocation failure returns `NULL` without changing
successful path resolution behavior.

`tests/automated/check-xmos-init-path-allocation.py` protects this contract.
