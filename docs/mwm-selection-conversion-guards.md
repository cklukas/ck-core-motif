---
nav_exclude: true
---

# MWM selection conversion defaults

`WMiConvert` now rejects null widget or output arguments before using them and
initializes `outputType` to `None` alongside the existing empty-buffer
defaults. Target-specific conversion branches can then set their documented
types, while invalid or unsupported requests return deterministic state.

The focused regression check is
`tests/automated/check-mwm-selection-conversion-guards.py`.
