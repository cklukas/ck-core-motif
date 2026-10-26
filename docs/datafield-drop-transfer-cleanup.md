---
nav_exclude: true
---
# DataField drop-transfer teardown

When a DataField drop-transfer closure no longer has a widget, the callback
now marks the transfer failed and releases any incoming value before returning.
This avoids leaking selection data during widget teardown.

Regression coverage: `tests/automated/check-datafield-drop-transfer-cleanup.py`.
