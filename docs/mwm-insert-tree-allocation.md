---
nav_exclude: true
---
# MWM command-tree path allocation

Client-command insertion builds a bracketed command path while walking the
MWM command tree.  The measured length is already bounded; the path buffer is
now also checked before formatting.  Allocation failure stops that insertion
without writing through a null pointer, while successful command matching and
path formatting remain unchanged.

`tests/automated/check-mwm-insert-tree-allocation.py` protects this contract.
