---
nav_exclude: true
---
# MWM `RemoveCommand` message lengths

`RemoveCommand` now validates its command ID, window count, and window-list
bytes before processing the request. Truncated IPC payloads are rejected
without reading beyond the received message; complete requests are unchanged.
