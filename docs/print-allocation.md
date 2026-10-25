---
nav_exclude: true
---
# Print setup allocation

Print-to-file and print-display-manager setup now check their small callback
context allocations.  Print-to-file closes its parent pipe before reporting
failure; PDM setup returns `XmPDM_NOTIFY_FAIL`.  Successful printing and
callback ownership are unchanged.

`tests/automated/check-print-allocation.py` protects this contract.
