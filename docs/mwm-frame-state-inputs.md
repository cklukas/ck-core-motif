---
nav_exclude: true
---
# MWM frame-state input validation

`SetFrameInfo` and `SetClientOffset` return without changing state for a null
client record.  `XBorderIsShowing` returns `False` for the same input.  These
are the safe no-state/no-visible-border results for the legacy APIs; valid
state updates and queries remain unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-state-inputs.py`.
