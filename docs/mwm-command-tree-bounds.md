---
nav_exclude: true
---
# Mwm command-tree bounds

Inline menu-command matching derives command paths and function arguments from
user-defined command-tree names.  The legacy matching path placed both strings
in fixed 1024-byte arrays, so long menu names could overflow stack storage.

The path now measures and allocates each rendered string, frees both after the
match is stored, and preserves the measured byte count as `size_t` through the
Xt `Cardinal` boundary. Existing command and cascade behavior is unchanged.

`tests/automated/check-mwm-command-tree-bounds.py` protects this contract.
