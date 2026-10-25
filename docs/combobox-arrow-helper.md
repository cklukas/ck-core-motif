---
nav_exclude: true
---
# ComboBox arrow helper guard

ComboBox arrow hit-area and drawing helpers now no-op for the plain ComboBox
type. Plain ComboBoxes do not expose an arrow; drop-down list and drop-down
combo-box arrow behavior is unchanged.

The regression check is
`tests/automated/check-combobox-arrow-helper.py`.
