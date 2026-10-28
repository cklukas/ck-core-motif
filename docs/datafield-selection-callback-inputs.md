---
nav_exclude: true
---

# DataField selection callback inputs

The DataField selection-reply callback now validates its widget, selection
closure, type, length, format, and non-empty payload before using them. It
defers display access until validation succeeds, releases rejected Xt-owned
data, and balances the pending selection reference on every rejected path.
Non-empty data tagged with the delete/NULL target atom is rejected as an
inconsistent reply rather than being interpreted as a delete operation.

Regression coverage:

```text
python3 tests/automated/check-datafield-selection-callback-inputs.py
```
