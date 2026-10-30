---
nav_exclude: true
---

# DataField context cleanup guards

DataField's X11 context cleanup callback now validates callback-owned context
data and its screen before calling `DisplayOfScreen` or X context APIs. A null
context is ignored; a context without a screen is released safely.

The focused source-contract check is:

```sh
python3 tests/automated/check-datafield-context-cleanup-guards.py
```
