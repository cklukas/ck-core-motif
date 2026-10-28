---
nav_exclude: true
---
# MWM enable/disable message lengths

MWM now validates the command ID, window-count, and window-list bytes before
processing `EnableCommand` or `DisableCommand`. Truncated IPC messages are
rejected before any count-driven unpack, while complete messages retain their
existing behavior.
