---
nav_exclude: true
---

# ToggleSwitch state API inputs

`XmToggleSwitchGetState` and `XmToggleSwitchSetState` now reject a null
widget before invoking the Xt subclass check. Invalid optional handles return
the neutral false/no-op result; valid switch state and callback behavior are
unchanged.

The focused regression check is
`tests/automated/check-toggle-switch-inputs.py`.
