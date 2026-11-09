---
nav_exclude: true
---

# MWM event-parser input validation

The exported MWM button and key event parsers now reject missing cursor or
output pointers before parsing.  Button-release parsing also validates the
parsed button number before indexing the fixed modifier-mask table.  Valid
button and key bindings keep their existing parsing and modifier behavior.

The focused regression is
`tests/automated/check-mwm-event-parser-inputs.py`.
