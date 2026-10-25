---
nav_exclude: true
---
# Simple widget name formatting

The Simple menu, radio-box, and check-box helpers generate child names such
as `button_0` and `separator_0`.  These names are derived from integer
counters and indexes and are written into a stack buffer before the child is
created.

The buffers now have 32 bytes of storage and all generated names use
`snprintf`.  Existing names are unchanged, while unusually large indexes or
long-running counter values cannot overwrite the stack.  The source
regression in `tests/automated/check-simple-widget-name-bounds.py` covers all
three helper paths and prevents an unbounded formatter from returning.
