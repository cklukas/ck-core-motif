---
nav_exclude: true
---
# DropDown callback lifecycle

DropDown interaction callbacks now reject incomplete callback data and missing
internal children before reading popup, text, or list state.  Arrow posting,
unposting, text verification, and custom verification hooks therefore become
safe no-ops after partial construction.

Fully constructed DropDowns retain the existing popup, verification, and
callback behavior.
