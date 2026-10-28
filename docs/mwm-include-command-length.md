---
nav_exclude: true
---
# MWM `IncludeCommand` message lengths

MWM now validates the received IPC length before unpacking the command header
and before reading the advertised window list. Truncated messages are rejected
without reading beyond the received buffer; complete messages retain the
existing include-command behavior.
