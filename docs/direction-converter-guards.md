---
nav_exclude: true
---

# Layout-direction converter guards

The shared layout-direction import/export converters now reject null widgets or
null value storage before invoking widget-class predicates or converting the
direction. Invalid exports leave caller storage untouched; invalid imports
return `XmSYNTHETIC_LOAD` without changing widget state.

The focused source-contract check is:

```sh
python3 tests/automated/check-direction-converter-guards.py
```
