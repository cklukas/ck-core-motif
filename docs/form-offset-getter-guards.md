---
nav_exclude: true
---

# Form offset getter guards

Form's synthetic top, bottom, left, right, and margin exports now validate the
child/widget class and output storage before reading attachment or margin
state. Invalid calls return deterministic zero values; normal offset and
margin conversion is unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-form-offset-getter-guards.py
```
