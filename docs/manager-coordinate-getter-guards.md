---
nav_exclude: true
---

# Manager coordinate getter guards

Manager's synthetic `x` and `y` exports now validate widget and output storage
before querying parent/shell geometry. Invalid calls return deterministic zero
values; normal manager coordinate conversion remains unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-manager-coordinate-getter-guards.py
```
