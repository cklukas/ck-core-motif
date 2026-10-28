---
nav_exclude: true
---
# MWM `DefineCommand` strings

MWM now decodes DefineCommand names only when each length-prefixed string fits
inside the received IPC payload. Truncated string data is rejected before
command-tree mutation, and temporary decoded strings are released after the
tree copies them. Complete requests retain their existing behavior.
