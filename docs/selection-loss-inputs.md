---
nav_exclude: true
---

# Selection-loss callback inputs

`ClipboardLoseProc` and `LoseProc` run when an owner loses a selection.  They
now reject a null widget, null selection pointer, or `None` selection atom
before deriving atoms, converting data, or removing destruction callbacks.
This keeps ownership teardown safe when a selection loss arrives during widget
destruction or cancellation.

The regression check is `tests/automated/check-selection-loss-inputs.py`.
