---
nav_exclude: true
---

# MWM help-argument declaration

`WmResParse.h` now declares `WmDtGetHelpArgs` with the same spelling and
output-buffer pointer types as its implementation. CDE-facing MWM code can
therefore include the parser interface without relying on an undeclared or
incompatible symbol.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-help-args-declaration.py
```
