---
nav_exclude: true
---

# ScrollBar value hook guards

ScrollBar's synthetic value export/import callbacks and processing-direction
default now validate widget and output storage before reading or changing
scrollbar state. Invalid exports return zero, invalid imports return
`XmSYNTHETIC_NONE`, and invalid defaults leave the `XrmValue` untouched.

The focused source-contract check is:

```sh
python3 tests/automated/check-scrollbar-value-hook-guards.py
```
