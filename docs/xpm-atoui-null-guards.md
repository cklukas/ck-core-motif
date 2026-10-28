---
nav_exclude: true
---

# XPM decimal-parser null guards

The internal XPM decimal parser now rejects null input and output pointers
before scanning or storing a value, preventing malformed parser calls from
dereferencing invalid addresses.

Regression: `tests/automated/check-xpm-atoui-null-guards.py`
