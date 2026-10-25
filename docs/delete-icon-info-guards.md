---
nav_exclude: true
---
# Icon-info deletion guards

`DeleteIconInfo` now ignores missing clients, icon-box state, or empty
placement storage before scanning the list.  Valid deletion still clears the
matching icon record.

`tests/automated/check-delete-icon-info-guards.py` protects the contract.
