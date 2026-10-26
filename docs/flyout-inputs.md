---
nav_exclude: true
---

# Flyout visibility API inputs

The public Flyout show, hide, and visibility APIs now reject a null widget
before invoking the Xt subclass check. Invalid optional handles return the
existing no-op or false result; valid popup, positioning, and dismissal
callback behavior is unchanged.

The focused regression check is `tests/automated/check-flyout-inputs.py`.
