---
nav_exclude: true
---
# Container button-motion input validation

Button-motion selection now returns safely when its widget or event is absent,
and handles a missing current-item constraint without dereferencing it. Valid
drag-selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-button-motion-inputs.py`.
