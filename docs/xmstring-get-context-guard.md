---
nav_exclude: true
---
# XmString context helper guards

`XmStringGetNextComponent` and `XmStringPeekNextComponent` now reject a null
`XmStringContext` with `XmSTRING_COMPONENT_END`. This keeps invalid or already
released contexts from reaching the internal component reader while preserving
normal iteration behavior.

The regression check is
`tests/automated/check-xmstring-get-context-guard.py`.
