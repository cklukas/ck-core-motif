---
nav_exclude: true
---
# Container clipboard widget-class guards

`XmContainerCut`, `XmContainerCopy`, and `XmContainerCopyLink` now reject
`NULL` and non-Container widget handles before reading selection state or
looking up an application context. Invalid callers receive `False`; valid
clipboard operations are unchanged.

The source contract is checked by
`tests/automated/check-container-clipboard-widget-class-guards.py`.
