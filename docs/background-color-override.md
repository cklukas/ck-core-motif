---
nav_exclude: true
---
# Legacy background color override

The private `_XmSetDefaultBackgroundColorSpec` entry point is used by legacy
integrations that need to select a background color outside the resource
database. Its previous implementation wrote to a function-local variable that
was never consulted by `_XmGetDefaultBackgroundColorSpec`, so the setter had no
effect.

Both functions now use one owned override string. The getter returns that
override when present and otherwise keeps the existing resource-database and
built-in fallback behavior. Replacing the override releases the previous copy,
and a null setter input selects the standard Motif default.

`tests/automated/check-background-color-override.py` guards the shared-storage
contract.
