---
nav_exclude: true
---

# DataField string getter guards

DataField's narrow and wide synthetic string exports now validate widget and
output storage before requesting a copied value. Invalid calls return a
deterministic null result; valid string and wide-string exports are unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-datafield-string-getter-guards.py
```
