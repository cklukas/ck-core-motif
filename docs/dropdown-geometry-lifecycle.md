---
nav_exclude: true
---
# DropDown geometry and action lifecycle

DropDown preferred-size calculation now supplies zero geometry for a missing
optional label, text, or arrow child instead of reading a null `child` pointer.
Cancel actions, combo-box lookup, and shell-handler registration likewise
reject missing widgets before walking parents or restoring text.

Complete DropDowns retain their existing sizing and popup behavior.
