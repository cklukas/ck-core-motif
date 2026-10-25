---
nav_exclude: true
---
# Icon-box increment guards

`UpdateIncrements` and `ResetArrowButtonIncrements` now return when their
icon-box, client, scrollbar, or increment state is incomplete.  Initialized
icon boxes continue to update scrollbar increments normally.

`tests/automated/check-arrow-increment-guards.py` protects the contract.
