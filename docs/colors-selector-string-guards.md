---
nav_exclude: true
---

# ColorSelector string getter guards

ColorSelector’s synthetic label and error-string getters now validate the
widget class and output slot before reading internal string storage. Invalid
callback inputs return a deterministic null value, while valid reads preserve
the existing copied `XmString` contract for all seven resources.

The focused source-contract check is
`python3 tests/automated/check-colors-selector-string-guards.py`.
