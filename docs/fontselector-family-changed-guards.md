---
nav_exclude: true
---
# FontSelector family-change callback guards

The family-change callback now validates its selector, source widget, font
state, and dropdown value. The no-change path releases the retrieved value
before returning, avoiding a repeated-selection leak.
