---
nav_exclude: true
---
# Container cursor-extension input guards

The Container cursor-extension action now rejects missing events, parameter
counts, empty parameter lists, and null parameter storage before using the
direction token or forwarding range selection.  Normal keyboard extension is
unchanged.

The source contract is checked by
`tests/automated/check-container-extend-cursor-input-guards.py`.
