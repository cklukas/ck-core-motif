---
nav_exclude: true
---

# DataField insertion callback inputs

The DataField selection-insertion and target-list callbacks now reject an
incomplete widget, closure, selection atom, event, type, length, or format
tuple before dereferencing it. Any Xt-owned selection result supplied with
that malformed callback is released first. Normal insertion and empty-value
handling are unchanged. Target-list iteration keeps the X11 length width and
rejects a nonzero length without a target array.

Regression coverage:

```text
python3 tests/automated/check-datafsel-insert-callback-inputs.py
```
