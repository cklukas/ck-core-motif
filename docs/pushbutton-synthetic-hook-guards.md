---
nav_exclude: true
---

# PushButton synthetic hook guards

PushButton and PushButtonGadget synthetic pixel conversion hooks now validate
the callback widget and `XtArgVal` storage before reading or converting values.
Invalid imports return `XmSYNTHETIC_NONE`; invalid exports return without
touching caller storage. Valid button subclasses retain their existing unit
conversion and default-shadow behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-pushbutton-synthetic-hook-guards.py
```
