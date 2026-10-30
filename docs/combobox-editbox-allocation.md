---
nav_exclude: true
---
# ComboBox edit-field allocation

ComboBox now stops initialization when its text-field child cannot be created.
The constructor returns cleanly before managing or asserting on the missing
child, and initialization registers the value-change callback only when the
edit field exists.
