---
nav_exclude: true
---

# TabStack tear-off menu selection guard

The optional tear-off menu callback now rejects a missing callback widget or
missing user-data target before selecting a tab. Valid menu entries continue to
call `XmTabStackSelectTab` exactly as before.

The focused source-contract check is
`python3 tests/automated/check-tabstack-menu-selection-guard.py`.
