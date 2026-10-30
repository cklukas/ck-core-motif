---
nav_exclude: true
---

# Label synthetic getter guards

Label's synthetic label, mnemonic, accelerator, and accelerator-text exports
now validate the output pointer and widget class before reading private Label
state. Invalid callback inputs return a deterministic null value; valid Label
subclasses retain their copied-string behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-label-synthetic-getter-guards.py
```
