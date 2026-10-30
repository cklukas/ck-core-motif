---
nav_exclude: true
---

# RowColumn synthetic getter guards

RowColumn's synthetic mnemonic, menu, option-label, and tear-off resources
now validate the callback output pointer and the source widget before reading
private RowColumn state. Invalid calls return a deterministic null value;
valid RowColumn subclasses retain their existing copied-string behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-rowcolumn-synthetic-getter-guards.py
```
