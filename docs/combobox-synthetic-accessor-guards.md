---
nav_exclude: true
---
# ComboBox synthetic accessor guards

ComboBox synthetic selected-item, selected-position, column, item, count, and
visible-count accessors now validate widget and value pointers before touching
child state. Import paths also return safely before initialization when the
list child is unavailable.
