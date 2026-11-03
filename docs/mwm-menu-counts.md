# MWM menu-button count safety

MWM stores the allocated and active menu-button counts as `unsigned int`.
Client-command updates now use the same unsigned type for their loop,
compaction, and snapshot counters. This avoids narrowing a valid count into a
signed temporary before indexing the menu-button array.

The focused source-contract regression is:

```sh
python3 tests/automated/check-mwm-menu-counts.py
```
