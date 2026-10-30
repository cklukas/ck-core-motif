---
nav_exclude: true
---
# Container outline-button sizing input validation

Outline-button sizing now returns safely for a missing Container widget or a
failed temporary gadget allocation, restoring the internal update flag on the
failure path. The outline-column synthetic resource also initializes its
container pointer only after validating the widget. Valid size measurement is
unchanged.

The source contract is checked by
`tests/automated/check-container-outline-size-inputs.py`.
