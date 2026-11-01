---
nav_exclude: true
---

# MWM keyboard-state mask typing

MWM removes locking modifiers and the XKB keyboard-group bits from incoming
keyboard and pointer state before matching bindings. The group mask is now
formed as an unsigned value, matching X11 state-mask types. This preserves the
existing group-bit behavior without relying on conversion of a negative signed
mask at each event comparison.
