---
nav_exclude: true
---
# MWM title fallback

When a managed client has no readable `WM_NAME`, MWM builds a title from its
client name. If that `XmString` allocation fails, it now falls back to the
existing shared default title before later measurement or drawing. The normal
localized title path is unchanged.

`tests/automated/check-mwm-title-fallback.py` protects this path.
