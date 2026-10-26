---
nav_exclude: true
---
# Dialog parent-input guards

`XmCreateDialogShell` and `XmeCreateClassDialog` now return `NULL` when their
parent widget is null, before shell creation or argument allocation. This
prevents malformed dialog-construction requests from reaching Xt with an
invalid parent while preserving valid dialog behavior.

The focused regression is
`tests/automated/check-dialog-parent-inputs.py`.
