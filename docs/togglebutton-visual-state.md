---
nav_exclude: true
---
# ToggleButton visual-state fallback

ToggleButton and ToggleButtonGadget painting now treats an invalid visual state
as a no-paint fallback. The normal unset, set, and indeterminate states retain
their existing rendering; malformed state does not terminate the application.

The regression check is
`tests/automated/check-togglebutton-visual-state.py`.
