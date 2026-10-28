---
nav_exclude: true
---
# MWM invoke-command input

`F_InvokeCommand` now parses its command ID, client window, and selection
fields individually with overflow and trailing-input checks. Values must fit
their protocol types; malformed input is rejected before any narrowing or
`SendInvokeMessage` call.
