---
nav_exclude: true
---
# XmList drag selected-position validation

Drag conversion now treats selected-position arrays as untrusted state. It
skips zero, negative, out-of-range, and null-item entries, and releases
partially built compound strings when an `XmString` copy fails. Valid selected
items retain their original order and separator behavior.
