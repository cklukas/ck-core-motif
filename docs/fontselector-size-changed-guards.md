---
nav_exclude: true
---
# FontSelector size-change callback guards

The size-change callback now validates its selector, source widget, current
font, and dropdown value before parsing or updating the point size. Valid
values retain the existing range checks and display refresh behavior.
