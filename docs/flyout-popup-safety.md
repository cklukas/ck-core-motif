---
nav_exclude: true
---
# Flyout popup safety

Flyout popup creation now checks shell, label-string, and label-widget
creation, destroying partial state on failure. Updating the popup label also
retains the existing label if a replacement `XmString` cannot be allocated.
Oversized popups are anchored at zero on screens too small to contain them.

`tests/automated/check-flyout-popup-safety.py` protects these paths.
