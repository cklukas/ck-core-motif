---
nav_exclude: true
---

# MWM automation input guard

`GetAutomationData` now rejects a null request payload before unpacking its
window and operation identifiers. Together with reply initialization, malformed
automation requests return an empty reply without dereferencing invalid input;
valid requests retain their existing protocol behavior.

The focused regression check is
`tests/automated/check-mwm-automation-input-guard.py`.
