---
nav_exclude: true
---
# XmString triple-reader context guard

`XmeStringGetComponent`, the shared implementation behind
`XmStringGetNextTriple`, now rejects a null context before reading context
state. It returns `XmSTRING_COMPONENT_END` just like other invalid-output
cases, preventing a null dereference for malformed or already-released
contexts.

The regression check is
`tests/automated/check-xmstring-triple-context.py`.
