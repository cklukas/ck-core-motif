---
nav_exclude: true
---
# Text selection callback inputs

The Text and TextField selection-transfer callbacks now validate their widget,
selection callback, and transfer closure inputs before deriving display or
widget state. Malformed or teardown-time callbacks therefore return safely
instead of dereferencing invalid state.

Regression coverage: `tests/automated/check-text-selection-callback-inputs.py`.
