---
nav_exclude: true
---
# DataField malformed transfer metadata

DataField drop-transfer callbacks now release incoming values even when target
metadata pointers are malformed. When a widget is available, the callback also
marks the transfer failed before returning.

Regression coverage: `tests/automated/check-datafield-drop-metadata-cleanup.py`.
