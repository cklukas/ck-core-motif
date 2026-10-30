---
nav_exclude: true
---
# ComboBox popup allocation

ComboBox now checks popup-shell creation before registering shell callbacks and
event handlers.  Its scrolled-list constructor also checks the returned list
before managing it, reading its parent, or querying scrollbar children.

Normal ComboBox and drop-down construction keeps the existing child names and
selection callbacks; allocation failures now return cleanly instead of
dereferencing a null child.
