---
nav_exclude: true
---
# Mwm icon-box title conversion

The icon-box shell now supplies `XmNtitle` and `XmNiconName` only when the
workspace title converts successfully from `XmString`.  A malformed title no
longer creates a shell with null title resources; the shell still opens with
its normal defaults.  Valid titles and the existing resource values are
unchanged.

`tests/automated/check-iconbox-title-conversion.py` protects this fallback.
