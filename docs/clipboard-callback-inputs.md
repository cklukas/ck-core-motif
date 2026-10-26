---
nav_exclude: true
---

# Clipboard-manager callback inputs

`ClipboardCallback` is invoked by the clipboard manager for snapshot and
delete notifications.  It now validates the widget, data ID, reason, and
target-dependent arguments before deriving the display or conversion context.
The dictionary-empty case is also handled after acquiring the process lock.
This prevents malformed or teardown-time callbacks from dereferencing a null
widget or entering the request table without valid input.

The regression check is
`tests/automated/check-clipboard-callback-inputs.py`.
