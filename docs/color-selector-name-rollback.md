---
nav_exclude: true
---
# ColorSelector color-name rollback

`color_name_changed` now checks the replacement allocation before updating the
widget.  A failed copy leaves the previous color name and color state intact;
the existing NULL operation still clears the name explicitly.

The focused source regression is
`tests/automated/check-color-selector-name-rollback.py`.
