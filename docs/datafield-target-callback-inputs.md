---
nav_exclude: true
---

# DataField target callback inputs

The DataField target-list callback now validates its widget, selection atom,
closure, event, and target payload before using them. It releases malformed
Xt-owned target data and uses the full X11 selection-length width when walking
the target list. Display and atom lookup occur only after widget validation.
The follow-up primary-selection request uses the normalized event timestamp,
including keyboard-originated requests.
The action helper also returns before atom/display access when its widget is
missing.

Regression coverage:

```text
python3 tests/automated/check-datafield-target-callback-inputs.py
```
