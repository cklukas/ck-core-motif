---
nav_exclude: true
---
# Icon-box focus callback guards

`ChangeActiveIconboxIconText` now returns for a missing icon widget or focus
event before reading the widget window or event type.  Valid focus transitions
remain unchanged.

`tests/automated/check-focus-callback-guards.py` protects the contract.
