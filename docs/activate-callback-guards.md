---
nav_exclude: true
---
# Icon activation callback guards

`IconActivateCallback` now returns for a missing widget and ignores a context
lookup that yields no client record before dispatching the restore action.
Valid icon activation remains unchanged.

`tests/automated/check-activate-callback-guards.py` protects the contract.
