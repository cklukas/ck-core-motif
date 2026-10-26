---
nav_exclude: true
---
# DataField drop-transfer inputs

DataField drop-transfer callbacks now validate the widget, transfer closure,
target type, length pointer, and transfer widget before reading or converting
incoming data. Malformed transfer notifications therefore return safely.

Regression coverage: `tests/automated/check-datafield-drop-transfer-inputs.py`.
