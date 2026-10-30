---
nav_exclude: true
---
# DatePicker popup destruction

The DatePicker registers a destroy callback on its transient popup shell.  If
the shell is destroyed by its parent or by application code, the DatePicker
clears both its popup-shell and calendar pointers and marks the popup closed.

This prevents a later `XmDatePickerOpen` call from reusing stale child widget
addresses.  Normal close and DatePicker teardown keep their existing behavior.
