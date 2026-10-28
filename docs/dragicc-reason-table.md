---
nav_exclude: true
---

# Drag ICC reason-table conversions

Drag-and-drop ICC reason lookup now iterates the static table with an unsigned
index matching `XtNumber`, and explicitly converts the validated table result
to the unsigned public return type. Invalid message types retain the existing
`_XmNUMBER_DND_CB_REASONS` sentinel.

Regression: `tests/automated/check-dragicc-reason-table.py`
