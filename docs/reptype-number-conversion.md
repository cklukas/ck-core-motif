---
nav_exclude: true
---
# Numeric representation action parameters

Motif actions that accept a representation type may retain the historical
numeric form (for example, `"2"`) in addition to a named value. Numeric input
is parsed with a range-checked conversion before it is validated against the
representation type. Inputs that exceed the range of an `int` are rejected
without invoking undefined integer conversion behavior; the accepted legacy
prefix syntax remains available.

The regression is run with:

```sh
python3 tests/automated/run-reptype-number-regressions.py /path/to/prefix /path/to/results
```
