---
nav_exclude: true
---
# Option-menu gadget lookup guards

`XmOptionLabelGadget` and `XmOptionButtonGadget` now return `NULL` when their
menu widget argument is null, before application-context lookup. Valid option
menus retain their existing child-discovery behavior.

The focused regression is
`tests/automated/check-option-gadget-inputs.py`.
