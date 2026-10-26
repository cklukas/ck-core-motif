# DataField paste buffer safety

`XmDataFieldPaste` now rejects clipboard lengths that cannot be represented by
Xt's `Cardinal` allocation size and checks the temporary buffer before asking
the clipboard subsystem to fill it. Failed or oversized pastes return `False`
without dereferencing a null buffer.

The regression check is:

```sh
python3 tests/automated/check-datafield-paste-buffer.py
```
