---
nav_exclude: true
---

# Column label getter guard

The Column synthetic label getter now validates the widget class and output
slot before reading its constraint label. Invalid callback inputs return a
deterministic null value; valid Column children retain the existing copied
`XmString` ownership contract.

The focused source-contract check is
`python3 tests/automated/check-column-label-getter-guard.py`.
