---
nav_exclude: true
---

# DataField insertion callback inputs

The DataField selection-insertion callback now rejects an incomplete widget,
closure, type, length, or format tuple before dereferencing it. Any Xt-owned
selection result supplied with that malformed callback is released first.
Normal insertion and empty-value handling are unchanged.

Regression coverage:

```text
python3 tests/automated/check-datafsel-insert-callback-inputs.py
```
