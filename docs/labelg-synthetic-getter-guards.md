---
nav_exclude: true
---

# LabelGadget synthetic getter guards

LabelGadget's synthetic label, mnemonic, accelerator, and accelerator-text
exports now validate the output pointer and gadget class before reading cached
compound-string state. Invalid callback inputs return deterministic null values;
valid LabelGadget subclasses retain their copied-string behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-labelg-synthetic-getter-guards.py
```
