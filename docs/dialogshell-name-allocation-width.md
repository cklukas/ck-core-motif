---
nav_exclude: true
---

# DialogShell name allocation width

`XmeCreateClassDialog` measures the caller’s name plus the DialogShell suffix
with `snprintf`. The measured length is now checked against Xt’s `Cardinal`
allocation width before creating the temporary name buffer; invalid or failed
allocation returns `NULL` without creating a partially named shell.

The regression is covered by `tests/automated/check-dialog-name-bounds.py`.
