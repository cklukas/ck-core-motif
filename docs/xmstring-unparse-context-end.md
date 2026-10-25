---
nav_exclude: true
---
# XmString unparse context-end guard

Unparse pattern matching now treats an unexpected end of the source context as
a malformed-input boundary. It stops skipping matched components without an
assertion failure; valid strings and parse mappings retain their existing
behavior.

The regression check is
`tests/automated/check-xmstring-unparse-context-end.py`.
