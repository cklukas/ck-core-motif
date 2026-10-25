---
nav_exclude: true
---
# MWM menu-spec copy failure handling

Custom-menu duplication depends on complete item and exclusion copies.  If
either list cannot be copied, duplication now releases the already-created
fields and returns failure instead of exposing a menu with silently missing
items or exclusions.  Successful custom menus retain the original contents
and ordering.

`tests/automated/check-mwm-menu-spec-copy-failure.py` protects this contract.
