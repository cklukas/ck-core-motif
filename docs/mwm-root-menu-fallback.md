---
nav_exclude: true
---
# MWM built-in root-menu fallback

When MWM constructs its built-in key or button bindings, a missing
`rootMenu` resource now falls back to `DefaultRootMenu` before measuring or
formatting the binding text. Configurations that provide a root menu retain
their existing behavior; incomplete resource sets no longer crash in `strlen`.

`tests/automated/check-mwm-root-menu-fallback.py` protects this contract.
