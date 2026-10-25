---
nav_exclude: true
---
# FontSelector encoding rollback

FontSelector now preserves the current encoding when a replacement string
cannot be allocated, both during SetValues migration and when a user selects a
new encoding. Successful changes retain the existing family refresh and menu
history behavior.

The focused source regression is
`tests/automated/check-fontselector-encoding-rollback.py`.
