---
nav_exclude: true
---

# Primitive tooltip hook guards

Primitive's tooltip synthetic import/export hooks now validate widget and value
storage before calling tooltip APIs. Invalid exports return a deterministic
null string; invalid imports return `XmSYNTHETIC_NONE` without changing state.

The focused source-contract check is:

```sh
python3 tests/automated/check-primitive-tooltip-hook-guards.py
```
