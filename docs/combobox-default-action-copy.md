---
nav_exclude: true
---
# ComboBox default-action copy failure

The ComboBox keyboard default-action bridge now dispatches its List callback
only after the edit value has been copied successfully.  Allocation failure
therefore cannot expose a NULL callback item or a pointer whose lifetime ends
when the bridge returns; the ComboBox still performs its normal selection
cleanup afterward.

The focused regression is
`tests/automated/check-combobox-default-action-copy.py`.
