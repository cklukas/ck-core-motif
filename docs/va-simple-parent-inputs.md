---
nav_exclude: true
---
# Varargs SimpleMenu parent guards

The varargs SimpleMenu constructors now return `NULL` for a null parent before
application-context lookup and argument processing:
`XmVaCreateSimpleMenuBar`, `XmVaCreateSimplePulldownMenu`,
`XmVaCreateSimplePopupMenu`, and `XmVaCreateSimpleOptionMenu`. Valid parent
widgets retain the existing varargs behavior.

The focused regression is
`tests/automated/check-va-simple-parent-inputs.py`.
