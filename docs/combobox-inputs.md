---
nav_exclude: true
---

# ComboBox item-setting API inputs

`XmComboBoxSetItem` now rejects a null widget before looking up the Xt
application context. Invalid optional handles remain a no-op; valid item
selection, scrolling, and callback behavior is unchanged.

The focused regression check is `tests/automated/check-combobox-inputs.py`.
