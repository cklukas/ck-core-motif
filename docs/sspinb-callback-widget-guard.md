---
nav_exclude: true
---

# SimpleSpinBox callback-widget guard

The SimpleSpinBox callback-widget helper now validates its widget class before
reading the text-field child. Invalid inputs return `NULL`; valid
SimpleSpinBox widgets continue to expose their text field to callback code.

The focused source-contract check is
`python3 tests/automated/check-sspinb-callback-widget-guard.py`.
