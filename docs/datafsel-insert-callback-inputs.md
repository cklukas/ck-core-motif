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
results are discarded before inspecting their first byte, and display access
is deferred until after widget validation. Invalid X11 format widths and
non-empty typeless results are rejected before conversion.
Invalid multibyte input is also rejected when converting locale text to wide
characters, before replacement receives the character count. Both conversion
paths preserve the native `size_t` result before narrowing.

Regression coverage:

```text
python3 tests/automated/check-datafsel-insert-callback-inputs.py
```
