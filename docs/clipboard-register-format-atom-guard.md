---
nav_exclude: true
---

# Clipboard format-registration atom guard

`RegisterFormat` now checks the atom returned for a format name before writing
the registration property. Allocation or interning failure returns
`ClipboardFail` without issuing an invalid X11 request; valid format
registrations are unchanged.

The focused regression check is
`tests/automated/check-clipboard-register-format-atom-guard.py`.
