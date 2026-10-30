---
nav_exclude: true
---

# ComboBox callback class guards

All ComboBox synthetic import and export callbacks now validate that the
callback widget is a ComboBox before accessing its private child state. Null
and non-ComboBox inputs are rejected using the existing callback sentinel or
return path, while valid ComboBox behavior is unchanged.

The focused source-contract check is
`python3 tests/automated/check-combobox-callback-class-guards.py`.
