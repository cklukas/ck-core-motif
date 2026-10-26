---
nav_exclude: true
---
# Compound-text property concatenation

Compound-text conversion now copies each segment at its known destination
offset. The text-list and fallback property paths no longer rescan the growing
result with `strcat`, so large multi-segment values retain the same output with
linear copy work.

The source contract is checked by:

```sh
python3 tests/automated/check-text-property-concat.py
```
