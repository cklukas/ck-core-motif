---
nav_exclude: true
---

# Accessibility name allocation rollback

`XmAccessibilityCreate` now destroys its partially initialized state and
returns NULL when copying the application name fails.  Valid names and the
public ABI are unchanged.

The static regression check is
`tests/automated/check-accessibility-name-allocation.py`.
