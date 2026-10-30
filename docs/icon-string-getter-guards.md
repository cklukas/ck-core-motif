---
nav_exclude: true
---

# Icon compound-string getter guards

IconGadget and IconButton now validate their synthetic label-string export
inputs before reading widget state. Null output pointers, null widgets, and
unrelated widget classes return safely with a deterministic null value; valid
icon widgets retain their existing copied `XmString` behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-icon-string-getter-guards.py
```
