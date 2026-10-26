---
nav_exclude: true
---
# ProgressRing label initialization

ProgressRing now falls back to an empty XmString when copying an initial label
fails. The widget remains safely renderable instead of carrying a null label
into metric and draw paths.

The focused source contract is checked by
`tests/automated/check-progress-ring-label-init.py`.
