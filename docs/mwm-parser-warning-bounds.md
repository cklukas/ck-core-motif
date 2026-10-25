---
nav_exclude: true
---
# Mwm parser-warning bounds

WSM resource parsing reports localized messages together with the current
configuration filename and line number.  The legacy `PWarning` path formatted
those values into a fixed `MAXWMPATH+1` buffer, so long parse messages or paths
could overwrite stack storage.

`PWarning` now measures and allocates the complete rendered message before
calling `_DtSimpleError`, checking the formatter width before converting to an
Xt `Cardinal`, with a direct message fallback if allocation fails.
The warning context and parser behavior are unchanged.

`tests/automated/check-mwm-parser-warning-bounds.py` protects this contract.
