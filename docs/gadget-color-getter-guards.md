---
nav_exclude: true
---

# Gadget color getter guards

Gadget’s synthetic highlight, top-shadow, and bottom-shadow color getters now
validate the gadget, output slot, parent, and manager class before reading
manager state. Invalid callback inputs return zero deterministically; valid
gadgets retain the inherited manager-color behavior.

The focused source-contract check is
`python3 tests/automated/check-gadget-color-getter-guards.py`.
