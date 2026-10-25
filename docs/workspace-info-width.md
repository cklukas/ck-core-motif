---
nav_exclude: true
---
# Mwm workspace-info property width

Mwm serializes workspace backdrop values into `_DT_WORKSPACE_INFO_*` properties.
The numeric scratch area now sizes each slot for the full native
`unsigned long` hexadecimal representation (`0x` plus all digits and the
terminator), rather than assuming a 16-byte slot. Each value is written with
`snprintf`, so 64-bit Atom and Window values cannot overwrite the following
property field on x86_64 while the property format remains unchanged.

`tests/automated/check-workspace-info-width.py` guards the slot-width and
bounded-formatting contract.
