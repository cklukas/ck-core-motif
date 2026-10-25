---
nav_exclude: true
---
# XmString tab conversion guards

The internal tab conversion helper now validates its display, screen-storage,
and drawable inputs before querying X. Invalid input returns zero pixels;
normal screen-aware tab conversion is unchanged.

The regression check is `tests/automated/check-xmstring-tab-conversion.py`.
