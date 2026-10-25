---
nav_exclude: true
---
# Icon insertion guards

`InsertIconInfo` now rejects missing icon-box or client state and verifies that
placement remains available after a growth attempt.  This prevents invalid
placement indices from reaching the icon-info array while preserving normal
insertion and expansion behavior.

`tests/automated/check-insert-icon-info-guards.py` protects the contract.
