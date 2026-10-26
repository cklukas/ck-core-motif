---
nav_exclude: true
---

# DataField picture initialization

During widget initialization, DataField now transfers ownership of a picture
source only after `XtNewString` succeeds. Allocation failure disables the
picture and modify-verify callback cleanly instead of parsing or later freeing
a null/non-owned pointer.

The focused source contract is checked by
`tests/automated/check-datafield-picture-init.py`.
