---
nav_exclude: true
---

# Gadget tooltip hook guards

Gadget's tooltip synthetic import/export hooks now validate widget and value
storage before calling tooltip APIs. Invalid exports return a deterministic
null string; invalid imports return `XmSYNTHETIC_NONE` without changing state.

The focused source-contract check is:

```sh
python3 tests/automated/check-gadget-tooltip-hook-guards.py
```
