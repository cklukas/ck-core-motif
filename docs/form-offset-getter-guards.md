---
nav_exclude: true
---

# Form offset getter guards

Form's synthetic top, bottom, left, and right offset exports now validate the
child widget, Form parent, constraint storage, and output pointer before
reading attachment state. Invalid calls return deterministic zero values;
normal attachment offset conversion is unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-form-offset-getter-guards.py
```
