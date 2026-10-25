---
nav_exclude: true
---

# MWM automation reply type

Successful `GetAutomationData` conversions now set `outputType` to
`_MOTIF_WM_AUTOMATION`, matching the target-typed replies produced by the WSM
conversion path. Empty failure replies continue to use `None`; the automation
payload format remains `WSM_PROTO_FMT`.

The focused regression check is
`tests/automated/check-mwm-automation-reply-type.py`.
