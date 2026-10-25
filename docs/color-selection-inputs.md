---
nav_exclude: true
---
# ColorObj selection callback input validation

`GetSelection` now rejects missing callback widgets, selection atoms, atom
storage, or screen metadata before scanning or writing state. It also avoids
dereferencing a missing transfer length when no property value is supplied.

`tests/automated/check-color-selection-inputs.py` protects this contract.
