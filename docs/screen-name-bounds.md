---
nav_exclude: true
---
# XmScreen generated name bounds

When `XmGetScreen` implicitly creates a screen widget, it derives a name from
the screen index. The fixed name buffer now uses `snprintf` with its capacity,
so the generated resource name cannot overrun the stack if the display exposes
an unusually large screen index.

`tests/automated/check-screen-name-bounds.py` guards this formatting contract.
