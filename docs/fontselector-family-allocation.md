---
nav_exclude: true
---
# FontSelector family allocation

The family DropDown builder now checks argument-list merging and child
creation before registering its verification and update callbacks. Temporary
multibyte text and merged arguments are released on every failure path.
