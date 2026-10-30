---
nav_exclude: true
---
# Flyout popup destruction

Flyout popup shells register a destroy callback.  If application code or a
parent shell destroys the transient shell, the Flyout clears its shell and
label pointers and marks itself hidden.

The next `XmFlyoutShow` therefore creates a fresh popup instead of attempting
to reuse freed widget addresses.
