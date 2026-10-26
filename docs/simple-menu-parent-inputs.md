---
nav_exclude: true
---
# SimpleMenu creation parent guards

The four SimpleMenu convenience constructors now return `NULL` for a null
parent before subresource lookup or application-context access:
`XmCreateSimpleMenuBar`, `XmCreateSimplePopupMenu`,
`XmCreateSimplePulldownMenu`, and `XmCreateSimpleOptionMenu`. Valid parent
widgets retain the existing convenience-menu behavior.

The focused regression is
`tests/automated/check-simple-menu-parent-inputs.py`.
