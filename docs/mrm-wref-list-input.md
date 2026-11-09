---
nav_exclude: true
---
# Mrm set-values reference list inputs

Set-values widget and callback reference builders now reject a null output-list
pointer before attempting initialization. The widget-reference path also
rejects a null widget name, preserving valid descriptor creation while
avoiding malformed-call crashes.

Deferred set-values update and resolution now also tolerate a null list or
empty list; resolution rejects a null current widget name before comparing it
with saved references.

The source-level regression is
`tests/automated/check-mrm-wref-list-input.py`.
