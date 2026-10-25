---
nav_exclude: true
---
# ColorSelector label initialization rollback

`XmColorSelector` now retains each existing label when an initialization
`XmStringCopy` fails.  Previously, a failed copy replaced the string-table
entry with NULL, even though child widgets had already received the original
label.  Keeping the original avoids inconsistent labels and makes later
updates and destruction safe under allocation pressure.

The focused regression is
`tests/automated/check-color-selector-label-init-rollback.py`.
