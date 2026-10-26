---
nav_exclude: true
---

# Virtual-key initialization inputs

`_XmVirtKeysInitialize` now validates that its handle is a non-null XmDisplay
before casting it or requesting its X display. Invalid internal calls return
without dereferencing the handle; normal display initialization is unchanged.

The focused regression check is `tests/automated/check-virtkeys-inputs.py`.
