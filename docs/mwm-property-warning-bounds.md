---
nav_exclude: true
---
# Mwm property-warning bounds

When a client title or icon title cannot be converted, `mwm` reports the X11
property atom or current locale.  The old diagnostic path formatted those
values into a fixed 200-byte array, so long localized messages or atom names
could overwrite stack storage and the `%.100s` limit could truncate useful
context.

Warnings are now measured with `snprintf`, allocated to their rendered length,
and released after dispatch.  Normal warning text and conversion fallback
behavior remain unchanged; atom names are also released after use.

`tests/automated/check-mwm-property-warning-bounds.py` protects this contract.
