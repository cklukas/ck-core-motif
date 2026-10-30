---
nav_exclude: true
---

# Gadget access-colors guards

The Gadget access-colors trait now validates the gadget, output structure, and
manager parent before reading manager color state. Invalid trait calls return
without dereferencing malformed state; valid gadgets receive the same color
mask and values as before.

The focused source-contract check is
`python3 tests/automated/check-gadget-access-colors-guards.py`.
