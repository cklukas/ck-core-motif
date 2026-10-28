---
nav_exclude: true
---

# DataField insertion callback inputs

The DataField selection-insertion and target-list callbacks now reject an
incomplete widget, closure, selection atom, event, type, length, or format
tuple before dereferencing it. Any Xt-owned selection result supplied with
that malformed callback is released first. Normal insertion and empty-value
handling are unchanged. Target-list iteration keeps the X11 length width and
rejects a nonzero length without a target array. The consumed target array is
released before the follow-up selection request is queued. Empty insertion
results are discarded before inspecting their first byte.

Regression coverage:

```text
python3 tests/automated/check-datafsel-insert-callback-inputs.py
```
