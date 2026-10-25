---
nav_exclude: true
---
# MWM command window-list allocation

The MWM include, enable, disable, rename, and remove command handlers now
reject empty window lists and allocation failure before reading or writing the
list. This prevents malformed client-command messages from dereferencing a
null list while leaving valid non-empty command dispatch unchanged.

`tests/automated/check-mwm-command-window-allocation.py` protects this
contract.
