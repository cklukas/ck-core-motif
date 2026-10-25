---
nav_exclude: true
---
# XmDropDown API null-safety

`XmDropDownGetValue`, `XmDropDownGetChild`, and the label, arrow, text, and
list getters now return `NULL` for null widgets before Xt context or subclass
checks. Valid DropDown child and value behavior remains unchanged.
