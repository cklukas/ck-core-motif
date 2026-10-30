---
nav_exclude: true
---

# Icon compound-string getter guards

IconGadget and IconButton now validate their synthetic label-string export and
padding-conversion inputs before reading widget state. Null output pointers,
null widgets, and unrelated widget classes return safely with deterministic
null/failed results; valid icon widgets retain their existing behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-icon-string-getter-guards.py
```

The padding-converter check is:

```sh
python3 tests/automated/check-iconbutton-padding-hook-guards.py
```
