---
nav_exclude: true
---
# FontSelector size allocation

The size DropDown builder now checks argument-list merging and child creation
before adding the control to the XLFD-only list or registering callbacks.
Temporary text and merged arguments are released on failure.
