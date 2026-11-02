---
nav_exclude: true
---

# MWM virtual-key binding lifetime

The virtual-key repair path copies X11 binding properties into Xt-owned,
null-terminated storage before parsing them. MWM now releases that storage only
with `XtFree` and exactly once on each path, avoiding allocator mismatches and
a fallback-path double free.

```sh
python3 tests/automated/check-mwm-virtkeys-binding-lifetime.py
```
