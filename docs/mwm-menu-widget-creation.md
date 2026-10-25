---
nav_exclude: true
---
# MWM menu widget creation

MWM now checks push-button gadget creation before applying sensitivity,
bookkeeping, or callbacks. If widget allocation fails, menu construction
returns through its existing failure path instead of dereferencing a null
child. Successful menu creation is unchanged.

`tests/automated/check-mwm-menu-widget-creation.py` protects this path.
