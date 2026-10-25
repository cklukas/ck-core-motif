---
nav_exclude: true
---
# Segmented-control button creation

`XmSegmentedControlSetItems` now checks each toggle-button creation before
installing callbacks or managing the child.  If creation fails, already
created buttons and copied item data are released and the control is reset to
an empty selection, avoiding a null-child dereference during layout.

`tests/automated/check-segmented-button-creation.py` protects this failure
path.
