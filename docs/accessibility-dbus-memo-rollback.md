---
nav_exclude: true
---
# Accessibility D-Bus memo rollback

The AT-SPI D-Bus transport now allocates replacement copies of cached
accessible names and value text before releasing the previous copies.  A
failed `XtNewString` therefore leaves the memo intact, preserving the
screen-reader name/value and the baseline used for subsequent change
notifications.

The focused regression is
`tests/automated/check-accessibility-dbus-memo-rollback.py`.
