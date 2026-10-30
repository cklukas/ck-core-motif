---
nav_exclude: true
---
# Container selection-range input validation

Selection-range, marquee, and visual-reset helpers now return safely when no
Container widget is supplied. Valid range and marquee selection behavior is
unchanged.

The source contract is checked by
`tests/automated/check-container-selection-range-inputs.py`.
