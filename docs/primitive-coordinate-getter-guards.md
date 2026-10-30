---
nav_exclude: true
---

# Primitive coordinate getter guards

Primitive's synthetic `x` and `y` exports now validate widget and output
storage before querying parent/shell geometry. Invalid calls return a
deterministic zero value; normal coordinate conversion remains unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-primitive-coordinate-getter-guards.py
```
