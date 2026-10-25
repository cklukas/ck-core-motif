---
nav_exclude: true
---
# Icon-box resize guards

`CheckIconBoxResize` now rejects missing clip-widget or scrollbar-policy state,
invalid placement dimensions, and negative content sizes before resizing.  Valid
resize behavior remains unchanged.

`tests/automated/check-iconbox-resize-guards.py` protects the contract.
