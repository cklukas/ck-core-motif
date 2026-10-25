---
nav_exclude: true
---
# XIM input-policy validation

XmIm now validates input-policy values before allocating an XIC. Invalid
values are ignored by registration, unregistration, and update dispatch rather
than reaching assertion-only branches. The supported per-shell, per-widget,
and inherited policies retain their existing behavior.

The regression check is `tests/automated/check-xim-input-policy.py`.
