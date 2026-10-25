---
nav_exclude: true
---
# Mwm command-tree bounds

Inline menu-command matching derives command paths and function arguments from
user-defined command-tree names.  The legacy matching path placed both strings
in fixed 1024-byte arrays, so long menu names could overflow stack storage.

The path now measures and allocates each rendered string, frees both after the
match is stored, and preserves the existing command and cascade behavior.

`tests/automated/check-mwm-command-tree-bounds.py` protects this contract.
