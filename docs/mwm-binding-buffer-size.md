---
nav_exclude: true
---

# MWM builtin binding buffer sizing

Builtin key and button bindings substitute the configured root-menu name into
their format strings. Both paths now check the combined format/name size before
converting it to Xt's allocation type. An unrepresentable name falls back to
the standard `DefaultRootMenu` name rather than wrapping the allocation.

Normal binding construction and parsing are unchanged. The focused regression
check is `tests/automated/check-mwm-binding-buffer-size.py`.
