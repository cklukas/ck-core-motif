---
nav_exclude: true
---
# Mrm hash-table initialization inputs

Hash-table initialization now returns immediately for a null table or
initialization flag, avoiding malformed-call dereferences. Valid one-time
initialization continues to clear the table and set its flag as before.

The source-level regression is `tests/automated/check-mrm-wci-hash-init.py`.
